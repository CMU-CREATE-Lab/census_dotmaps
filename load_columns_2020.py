# %%
import json, os, struct, sys
from functools import cache
from psql_utils.epsql import Engine
import pandas as pd
import numpy as np
from tempfile import NamedTemporaryFile
from census_utils import canonicalize_census_table_name, canonicalize_census_column_name

@cache
def engine():
    return Engine()

# %%
import gzip

dest_dir = "census2020_block2020"

# Read from block_geoids_2020.json.gz
geoids_2020 = json.load(gzip.open('block_geoids_2020.json.gz'))

# %%
def create_table_column_files(table_sql_name: str):
    table_name = table_sql_name.split('_')[1]
    canonical_table_name = canonicalize_census_table_name(table_name)

    prefix = f'{table_name}_'
    sql_cols = engine().table_columns(table_sql_name)
    col_infos = []
    for sql_col in sql_cols:
        if sql_col.startswith(prefix):
            col = canonicalize_census_column_name(sql_col[len(prefix):])
            col_infos.append({
                "sql_col": sql_col,
                "filename": f"{dest_dir}/{canonical_table_name}{col}.2020.float32"
            })
    for col_info in col_infos:
        if not os.path.exists(col_info["filename"]):
            break
    else:
        print(f"{table_sql_name} already done, skipping")
        return
    
    # TODO: using the "ohio" package like this, it's about 3x faster (3 min instead of 9 min on hal21)
    # df = pd.DataFrame.pg_copy_from(f"select * from {table_sql_name} order by geoid", engine(), dtype={"state":str, "county":str, "tract":str, "block":str, "geoid":str})

    df = engine().execute_returning_df(f"select * from {table_sql_name} order by geoid")
    table_geoids = list(df["geoid"])
    assert table_geoids==geoids_2020

    for col_info in sorted(col_infos, key=lambda x: x["sql_col"]):
        sql_col = col_info["sql_col"]
        filename = col_info["filename"]
        data = np.array([0] + df[sql_col].tolist(), dtype=np.float32)

        # Atomically create dest_filename        
        with NamedTemporaryFile('wb', dir=dest_dir, delete=False) as f:
            f.write(data.tobytes())
        os.rename(f.name, filename)

        filelen = os.stat(filename).st_size
        assert filelen == (len(df) + 1) * 4
        print("Wrote %s (%d bytes)" % (filename, filelen))

# NOTE: dec2020pl tables were preliminary, and names conflict with the final dec2020dhc tables
tables = [tab for tab in engine().list_tables('census') if tab.startswith('dec2020dhc_') and tab.endswith('_block')]
tables = sorted(tables)
len(tables)


# %%


for table_sql_name in tables:
    create_table_column_files(f"census.{table_sql_name}")

# %%



