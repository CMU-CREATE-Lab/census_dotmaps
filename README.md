
To create new columns, run load_columns_2020.py, editing the "tables =" line to control which tables are loaded.  The notebook will create a new vector in the "census2020_block2020" directory for each table's column.

Afterwards, rsync the columns to hal15

The "screen" utility can be useful for managing long-running processes on the server.  See https://linuxize.com/post/how-to-use-linux-screen/ for a tutorial.



