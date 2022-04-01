import Store from "../store/store";

const vt = v => {
    if (typeof v === 'string') return v.endsWith('Op') ? v.substring(0, v.length - 2) : v;
    if (!v) return v;
    return Object.fromEntries(Object.entries(v).map(([key, val]) => [vt(key), val]));
}
const ht = h => h.map(a => vt(a));

export function tacticTransformer(t, tId) {
    let value_count = 0;
    t.tactic.forEach(h => h.forEach(a => a && (value_count++)));
    return {
        ...t,
        tactic: t.tactic.map(ht),
        value_count,
        tactic_surrounding: t.tactic_surrounding.slice(1, t.tactic_surrounding.length - 1).map(ht),
        fixId: tId,
        fix: false,
    }
}

export function rallyTransformer(r, tId) {
    return {
        ...r,
        rally: r.rally.map(ht),
        index: r.index[tId][0],
    }
}

function getTacticId(index) {
    const tactics = Store.getStores().analysis.sortedTactics;
    return index
        .split(',')
        .map(a => parseInt(a.trim()))
        .map(i => tactics[i - 1].id);
}

export function constraintTransformer(type, params) {
    return [
        type,
        {
            ...params,
            index: !params.index ? undefined : getTacticId(params.index),
        }
    ]
}
