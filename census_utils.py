import re

# convert e.g. p35e to p035e
def canonicalize_census_table_name(name):
    match = re.match(r'([a-z]+)(\d+)([a-z]*)$', name, re.IGNORECASE)
    if not match:
        raise ValueError(f'Invalid table name: {name}')
    (prefix, num, suffix) = match.groups()
    if not suffix:
        suffix = ''
    return f'{prefix}{int(num):03d}{suffix}'.upper()

def canonicalize_census_column_name(name):

    match = re.match(r'(\d+)([a-z]+)?$', name, re.IGNORECASE)
    if not match:
        raise ValueError(f'Invalid column name: {name}')
    (num, suffix) = match.groups()
    if not suffix or suffix == 'n':
        suffix = ''

    return f'{int(num):04d}{suffix}'.upper()
