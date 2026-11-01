import os, sys, json, collections, csv
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import phase_b_scan as P
import phase_b_classify as C
S = sys.argv[1]
d = C.load(S)
pre_only = [t for t in d['occ'] if t in d['dbg'] and t in d['pre'] and not P.PLACEHOLDER.match(t) and t not in P.CPP_WORDS]
pre_multi = [t for t in pre_only if len(P.words(t)) >= 2]
retail_excluded = [t for t in d['occ'] if t in d['retail'] and len(P.words(t)) >= 2 and t not in d['pre']]
rows = list(csv.DictReader(open(os.path.join(S, 'phase-b-rename-map.tsv'), encoding='utf-8'), delimiter='\t'))
ref = [(r['current_name'], r['proposed_name']) for r in rows if r['kind'] == 'reference' and r['class'] == 'RENAME']
print(json.dumps({'debug_matches_predating_debug_build': len(pre_only), 'of_which_multiword': len(pre_multi),
                  'reference_renames': ref}, indent=1))
