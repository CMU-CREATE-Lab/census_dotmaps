import re

# convert e.g. p35e to p035e
def canonicalize_census_table_name(name):
    (prefix, num, suffix) = re.match(r'([a-z]+)(\d+)([a-z])?$', name).groups()
    if not suffix:
        suffix = ''
    return f'{prefix}{int(num):03d}{suffix}'.upper()

def canonicalize_census_column_name(name):
    (num, suffix) = re.match(r'(\d+)([a-z]+)?$', name).groups()
    if not suffix or suffix == 'n':
        suffix = ''

    return f'{int(num):04d}{suffix}'.upper()
