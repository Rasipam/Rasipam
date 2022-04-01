import Store from "./store";
import VirtualData, {values, virtualRally} from "../utils/virtualData";

const url = uri => `http://127.0.0.1:8000${uri}`;

class BaseAPI {
    token = null;
    headers = new Headers();

    constructor() {
        this.headers.set('Content-type', 'application/json')
    }

    confirmToken = () => new Promise((resolve, reject) => {
        if (this.token !== null) resolve();
        else this.init().catch(reject).then(resolve);
    });

    fetch = (a, b) => this.confirmToken()
        .then(() => {
            const headers = new Headers(this.headers);
            b && b.headers && Object.entries(b.headers).forEach(([key, value]) => headers.set(key, value));
            return fetch(a, {
                headers: headers,
                ...(b || {})
            })
        })
        .catch(err => {
            console.error(err);
            Store.getStores().system.setNetwork(false);
        })
        .then(res => {
            Store.getStores().system.setNetwork(true);
            return res;
        });

    init = () => fetch(url('/token'))
        .catch(() => Store.getStores().system.setNetwork(false))
        .then(res => res.json())
        .then(res => {
            this.token = res;
            this.headers.set('Token', res);
            Store.getStores().system.setNetwork(true);
        });
}

class API extends BaseAPI {
    getDatasets = () => this.fetch(url('/datasets'))
        .then(res => res.json())

    setDataset = (dataset, player, opponents) => {
        console.log(JSON.stringify({dataset, player, opponents}))
        return this.fetch(
        url('/dataset'),
        {
            method: 'POST',
            body: JSON.stringify({dataset, player, opponents})
        })
        .then(res => {
            console.log(res);
            return res.json()})}

    runAlg = () => this.fetch(
        url('/tactic'),
        {method: 'POST'}
    ).then(res => res.json())

    getTacticSequences = tac_id => this.fetch(
        url(`/rally/${tac_id}`)
    ).then(res => res.json())

    processText = text => this.fetch(
        url(`/text/${text}`)
    ).then(res => res.json())

    processAudio = audio => new Promise(resolve => {
        resolve('');
    })

    modify = (type, params) => this.fetch(
        url('/modification'),
        {method: 'POST', body: JSON.stringify({type, params})}
    ).then(res => res.json())

    undo = () => this.fetch(
        url('/modification'),
        {method: 'DELETE'}
    ).then(res => res.json())

    fixTactic = (tac_id, isFix) => this.fetch(
        url(`/tactic/preference/${tac_id}`),
        {method: 'PUT', body: JSON.stringify(isFix)},
    ).then(res => res.json())

    videoSrc = videoName => url(`/video/${videoName}`)
}

class VirtualAPI extends BaseAPI {
    lastTacticSet = null;

    getDatasets = () => new Promise(resolve => {
        resolve(['Badminton', 'Tennis', 'Table Tennis'].map(sport => ['Female', 'Male'].map(gender => ({
            name: `${sport} (${gender})`,
            matches: [
                {
                    name: '',
                    players: ['a', 'b'],
                    sequenceCount: 100,
                }
            ],
            attrs: Object.keys(values[sport]),
        }))).flat())
    })

    setDataset = (dataset, player, opponents) => this.fetch(
        url('/dataset'),
        {
            method: 'POST',
            body: JSON.stringify({dataset, player, opponents})
        })
        .then(res => res.json())

    runAlg = () => new Promise(resolve => {
        this.lastTacticSet = VirtualData.tacticsSet(null);
        resolve(this.lastTacticSet)
    })

    getTacticSequences = tac_id => new Promise(resolve => {
        const t = this.lastTacticSet.tactics.find(t => t.id === tac_id);
        resolve([...Array(t.usage_count)].map(() => virtualRally(tac_id, t.tactic.length, t.user)))
    })

    processText = text => new Promise(resolve => {
        resolve({
            type: 'LimitIndex',
            params: {
                min: 1,
                max: 3,
            }
        })
    })

    processAudio = audio => new Promise(resolve => {
        resolve('Hello World');
    })

    modify = (type, params) => new Promise(resolve => {
        this.lastTacticSet = VirtualData.tacticsSet(this.lastTacticSet);
        resolve(this.lastTacticSet)
    })

    undo = () => new Promise(resolve => resolve(true))

    fixTactic = (tac_id, isFix) => new Promise(resolve => resolve(true))

    videoSrc = videoName => './video.mp4'
}

const api = window.is_dev() ? new VirtualAPI() : new API();

export default api;
