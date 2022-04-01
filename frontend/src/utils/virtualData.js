import uuid4 from "./uuid4";
import randomInt from "./randomInt";
import Store from "../store/store";

export const values = {
    'Badminton': {
        'Ball Height': [
            'Very low',
            'Low',
            'Medium',
            'High'
        ],
        'Ball Position': [
            'Backcourt Left',
            'Backcourt Right',
            'Midfield Left',
            'Midfield Right',
            'Forecourt Left',
            'Forecourt Right'
        ],
        'Hit Technique': [
            'Net service',
            'Backcourt service',
            'Smash',
            'Drive',
            'Lift',
            'High clear',
            'Hook',
            'Shot',
            'Net shot',
            'Drop',
            'Push',
            'Block',
            'Other',
            'Score'
        ],
    },
    'Tennis': {
        'Ball Position': [
            'Starting Tee 1',
            'Starting Tee 2',
            'Starting Tee 3',
            'Starting Tee 4',
            'Serving right zone outside corner',
            'Serving right zone midway',
            'Serving right zone inside corner',
            'Serving left zone outside corner',
            'Serving left zone midway',
            'Serving left zone inside corner',
            'Right zone front',
            'Right zone midfront',
            'Right zone midback',
            'Right zone back',
            'Midright zone front',
            'Midright zone midfront',
            'Midright zone midback',
            'Midright zone back',
            'Midleft zone front',
            'Midleft zone midfront',
            'Midleft zone midback',
            'Midleft zone back',
            'Left zone front',
            'Left zone midfront',
            'Left zone midback',
            'Left zone back',
            'RF RMF MRF MRMF no drop point',
            'RB RMB MRB MRMB no drop point',
            'LF LMF MLF MLMF no drop point',
            'LB LMB MLB MLMB no drop point',
            'Unexpected ball',
        ],
        'Hitting Pose': [
            'Starting Tee 1',
            'Starting Tee 2',
            'Starting Tee 3',
            'Starting Tee 4',
            'Forehand',
            'Backhand',
            'Sideway',
            'Back sideway',
            'Untouch',
        ],
        'Hit Technique': [
            'Overhand serving',
            'Underhand serving',
            'Drive',
            'Drive in the air',
            'Smash',
            'Ground smash',
            'Volley',
            'Drop shot',
            'Push and block',
            'Half volley',
            'Lob',
            'Slice',
            'Other',
        ],
    },
    'Table Tennis': {}
}

function genValue(ds, attrKey, count = 1) {
    const availVal = values[ds][attrKey];
    if (count === 1) return availVal[randomInt(availVal.length)];

    const res = {};
    let remainCount = count;
    while (remainCount !== 0) {
        const c = randomInt(1, remainCount + 1);
        let val = availVal[randomInt(availVal.length)];
        while (Object.keys(res).length < 2 && res.hasOwnProperty(val))
            val = availVal[randomInt(availVal.length)];
        if (!res.hasOwnProperty(val)) res[val] = 0;
        res[val] += c;
        remainCount -= c;
    }
    return res;
}

function genHit([single, multi], count = 1, ds, attrs = []) {
    if (!ds)
        ds = Store.getStores().analysis.dataset;
    for (const dsName of Object.keys(values))
        if (ds.startsWith(dsName))
            ds = dsName;
    if (!attrs)
        attrs = Store.getStores().analysis.attrs;
    return attrs.map(attrKey => {
        if (Math.random() < single / (single + multi)) return genValue(ds, attrKey, 1);
        else return genValue(ds, attrKey, count)
    })
}

export function virtualTactic(ds, attrs) {
    if (!ds)
        ds = Store.getStores().analysis.dataset;
    for (const dsName of Object.keys(values))
        if (ds.startsWith(dsName))
            ds = dsName;
    if (!attrs)
        attrs = Store.getStores().analysis.attrs;

    const seq_count = randomInt(50, 100);
    const win_seq_count = randomInt(10, seq_count);
    const double_usage_count = randomInt(5, seq_count);
    const double_usage_win_seq_count = randomInt(Math.min(win_seq_count, double_usage_count));
    const usage_count = seq_count + double_usage_count;

    const tactic = [...new Array(randomInt(2, 7))].map(() => genHit([0.6, 0.4], usage_count, ds, attrs))

    return {
        id: uuid4(),

        tactic: tactic.map(hit => hit.map(val => (typeof val === 'string') ? val : null)),
        tactic_surrounding: tactic.map(hit => hit.map(val => (typeof val !== 'string') ? val : null)),
        user: randomInt(2),

        seq_count,
        win_seq_count,
        usage_count,
        win_usage_count: win_seq_count + double_usage_win_seq_count,

        x: Math.random(),
        y: Math.random(),
    }
}

const duration = randomInt(40 * 60, 90 * 60);

export function virtualRally(tacticId, tacticLen, tacticUser, ds, attrs) {
    if (!ds)
        ds = Store.getStores().analysis.dataset;
    for (const dsName of Object.keys(values))
        if (ds.startsWith(dsName))
            ds = dsName;
    if (!attrs)
        attrs = Store.getStores().analysis.attrs;

    const is_server = randomInt(2) === 0;
    const isEven = (tacticUser === 0) === is_server;
    const hit_count = randomInt(tacticLen + (isEven ? 0 : 1), 10);
    const start_time = Math.random() * (duration - 5);

    return {
        id: uuid4(),
        win: randomInt(2) === 0,
        is_server,
        hit_count,

        index: {
            [tacticId]: Math.floor(randomInt(hit_count - tacticLen) / 2) * 2 + (isEven ? 0 : 1)
        },
        rally: [...new Array(hit_count)].map(() => genHit([1, 0], 1, ds, attrs)),

        match_name: 'video',
        video_name: 'video.mp4',
        start_time,
        end_time: Math.random() * 5 + start_time,
    }
}

export default class VirtualData {
    static tacticsSet = (oldSet = null) => {
        if (oldSet === null)
            return {
                desc_len: randomInt(150, 250),
                tactics: [...new Array(randomInt(17, 24))].map((_, i) => virtualTactic()),
            }
        else {
            const tactics = JSON.parse(JSON.stringify(oldSet.tactics));
            const modType = randomInt(3);

            if (modType === 0) {
                const oldId = randomInt(tactics.length);
                // const oldT = tactics[oldId];
                tactics.splice(oldId, 1);
                const newT = [...new Array(randomInt(2, 5))].map((_, i) => virtualTactic());
                tactics.push(...newT);
            } else if (modType === 1) {
                const oldId = randomInt(tactics.length);
                // const oldT = tactics[oldId];
                tactics.splice(oldId, 1);
                const newT = virtualTactic();
                tactics.push(newT);
            } else {
                // const oldT = [];
                for (let i = randomInt(2, Math.min(tactics.length, 5)); i--;) {
                    let oldId = randomInt(tactics.length);
                    // oldT.push(tactics[oldId]);
                    tactics.splice(oldId, 1);
                }
                const newT = virtualTactic();
                tactics.push(newT);
            }

            return {
                desc_len: oldSet.desc_len + randomInt(-10, 10),
                tactics,
            }
        }
    }
}
