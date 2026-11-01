"""Print rows of the phase B map: phase_b_view.py <map.tsv> <class> <kind,kind> [width]"""
import csv, sys
path, cls, kinds = sys.argv[1], sys.argv[2], sys.argv[3].split(',')
width = int(sys.argv[4]) if len(sys.argv) > 4 else 100
for r in csv.DictReader(open(path, encoding='utf-8'), delimiter='\t'):
    if (cls == '*' or r['class'] == cls) and (kinds == ['*'] or r['kind'] in kinds):
        print(r['kind'], r['current_name'], '->', r['proposed_name'], '|', r['reason'][:width], '|', r['location(s)'][:60])
