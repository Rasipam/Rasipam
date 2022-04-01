import {makeAutoObservable} from "mobx";
import Store from "./store";
import api from "./api";
import tacticSorter, {SortTypes} from "../utils/tacticSort";
import {genTacticStat, mergeStat} from "../utils/tacticStat";
import {constraintTransformer, rallyTransformer, tacticTransformer} from "../utils/dataTransformer";

export default class AnalysisStore {
    dataset = '';
    player = '';
    opponents = [];
    setDataset = ds => {
        this.dataset = ds;
        this.player = '';
        this.opponents = [];
    }
    setPlayer = p => {
        this.player = p;
        if (this.opponents.includes(p))
            this.opponents = this.opponents.filter(op => op !== p);
    }
    setOpponents = ops => this.opponents = ops;

    get attrs() {
        const datasets = Store.getStores().data.datasets;
        for (const ds of datasets) {
            if (ds.name === this.dataset)
                return ds.attrs;
        }
        return [];
    }

    restart = () => {
        const history = {
            lastUpdate: new Date(),
            query: null,
            desc_len: null,
            tactics: null,
            sequences: null,
        }
        this.initCacheState();
        this.clearOldData();
        api.setDataset(this.dataset, this.player, this.opponents)
            .then(api.runAlg)
            .then(res => {
                history.desc_len = res.desc_len;
                history.tactics = res.tactics.map(tacticTransformer);
                return Promise.all(res.tactics.map(t => api.getTacticSequences(t.id)))
            })
            .then(res => {
                history.sequences = Object.fromEntries(history.tactics.map((t, tid) => [
                    t.id,
                    res[tid].map(r => rallyTransformer(r, t.id)),
                ]));
                this.initHistory(history);
            });
    }

    history = [];
    pushHistory = state => {
        this.history.push(state);
        this.viewHistory(this.history.length - 1);
    }
    popHistory = () => {
        this.history.pop();
        this.viewHistory(this.history.length - 1);
    }
    initHistory = (history) => {
        this.history = [history];
        this.currentViewHistory = 0;
    }
    undo = () => {
        api.undo()
            .then(this.popHistory)
    }

    cacheState = {
        query: {},
        tactics: [],
        sequences: {},
        desc_len: 0,
    }
    get isPreviewing() {
        return this.cacheState.desc_len !== 0;
    }
    initCacheState = () => this.cacheState = {
        query: {},
        tactics: [],
        sequences: {},
        desc_len: 0,
    }
    setCacheState = s => this.cacheState = s;
    preview = query => {
        const state = {
            query,
            desc_len: 0,
            tactics: [],
            sequences: {}
        }
        api.modify(...constraintTransformer(query.type, query.params))
            .then(res => {
                state.desc_len = res.desc_len;
                state.tactics = res.tactics.map(tacticTransformer);
                return Promise.all(res.tactics.map(t => api.getTacticSequences(t.id)));
            })
            .then(res => {
                state.sequences = Object.fromEntries(state.tactics.map((t, tid) => [
                    t.id,
                    res[tid].map(r => rallyTransformer(r, t.id)),
                ]))
                this.setCacheState(state);
            })
    }
    applyChange = () => {
        this.pushHistory({
            lastUpdate: new Date(),
            ...this.cacheState,
        })
        this.initCacheState();
    }
    cancelChange = () => {
        api.undo()
            .then(this.initCacheState);
    }

    currentViewHistory = -1;
    viewHistory = idx => this.currentViewHistory = idx;

    get stateEditable() {
        return this.history.length - 1 === this.currentViewHistory;
    }

    get state() {
        return this.history[this.currentViewHistory] || {
            query: null,
            desc_len: 0,
            tactics: [],
            sequences: {},
        };
    }

    selectedTactics = [];
    selectTactic = (id, selected) => {
        if (selected && !this.selectedTactics.includes(id)) this.selectedTactics.push(id);
        if (!selected) {
            const idx = this.selectedTactics.indexOf(id);
            if (idx > -1) this.selectedTactics.splice(idx, 1);
        }
    }

    favoriteTactics = [];
    setFavoriteTactics = t => this.favoriteTactics = t;
    favoriteTactic = (id, favor) => {
        api.fixTactic(this.state.tactics.find(t => t.fixId === id).id, favor)
            .then(r => {
                if (favor && !this.favoriteTactics.includes(id)) this.setFavoriteTactics([...this.favoriteTactics, id]);
                if (!favor) this.setFavoriteTactics(this.favoriteTactics.filter(t => t.fixId !== id));
            })
    }

    hoveredTactic = null;
    hoverTactic = (id, hover) => {
        if (hover) this.hoveredTactic = id;
        else if (this.hoveredTactic === id) this.hoveredTactic = null;
    }
    clearOldData = () => {
        this.selectedTactics = [];
        this.favoriteTactics = [];
        this.hoveredTactic = null;
    }

    get statTactics() {
        const globalStat = genTacticStat();
        return this.state.tactics.map(tactic => {
            const stat = genTacticStat(tactic);
            mergeStat(globalStat, stat);
            return {
                ...tactic,
                stat,
                globalStat,
            }
        })
    }

    getStatTactics = (state) => {
        const globalStat = genTacticStat();
        return state.tactics.map(tactic => {
            const stat = genTacticStat(tactic);
            mergeStat(globalStat, stat);
            return {
                ...tactic,
                stat,
                globalStat,
            }
        })
    }

    sortType = SortTypes.ImportanceDown;
    setSortType = newSortType => this.sortType = newSortType;
    get sortedTactics() {
        return this.statTactics.map(t => t).sort(tacticSorter[this.sortType])
    }

    get ralliesOfSelectedTactics() {
        const rallies = [];

        const tactics = this.state.tactics;
        const sequences = this.state.sequences;
        this.selectedTactics.forEach(id => {
            const tactic = tactics[id];
            const seqs = sequences[tactic.id] || [];
            rallies.push(...seqs.map(seq => ({
                ...seq,
                tacticPos: [seq.index, seq.index + tactic.tactic.length]
            })))
        })

        return rallies;
    }

    get previewTactics() {
        const statTactics = this.sortedTactics;
        const cacheStatTactics = this.getStatTactics(this.cacheState).sort(tacticSorter[this.sortType]);
        const deleteTactics = statTactics.map((t, i) => ({...t, sortedIndex: i, newSortedIndex: -1})).filter(statTactic => {
                const cacheStatTacticIndex = cacheStatTactics.findIndex(cacheStatTactic => cacheStatTactic.id === statTactic.id);
                return cacheStatTacticIndex === -1;
            }
        );
        const newTactics = cacheStatTactics.map((t, i) => ({...t, sortedIndex: -1, newSortedIndex: i})).filter(cacheStatTactic => {
                const statTacticIndex = statTactics.findIndex(statTactic => statTactic.id === cacheStatTactic.id);
                return statTacticIndex === -1;
            }
        )
        const keepTactics = statTactics.map((t, i) => ({...t, sortedIndex: i, newSortedIndex: -1})).filter(statTactic => {
                const cacheStatTacticIndex = cacheStatTactics.findIndex(cacheStatTactic => cacheStatTactic.id === statTactic.id);
                statTactic.newSortedIndex = cacheStatTacticIndex;
                return cacheStatTacticIndex !== -1;
            }
        );
        return [deleteTactics.concat(newTactics).concat(keepTactics),
            deleteTactics.length, deleteTactics.length + newTactics.length];
    }

    constructor() {
        makeAutoObservable(this);
        Store.register('analysis', this);
    }

    init = () => new Promise((resolve, reject) => {
        // let lastUpdate = new Date();
        // this.pushHistory({
        //     lastUpdate,
        //     desc_len: 200,
        //     query: null,
        //     tactics: [],
        //     sequences: {},
        // });
        //
        // if (window.is_dev()) {
        //     let lastDescriptionLength = 200;
        //     for (let i = 0; i < 13; i++) {
        //         lastUpdate = new Date(lastUpdate);
        //         lastUpdate.setMinutes(lastUpdate.getMinutes() + randomInt(3, 6));
        //         lastDescriptionLength += randomInt(-5, 10);
        //
        //         const tactics = [...new Array(randomInt(17, 24))].map((_, i) => ({
        //             ...virtualTactic(),
        //             fix: false,
        //             fixId: i,
        //         }));
        //         const sequences = {};
        //         tactics.forEach(t => sequences[t.id] = [...Array(t.usage_count)].map(() => {
        //             const rally = virtualRally(t.id, t.tactic.length);
        //             rally.index = rally.index[0][1];
        //             return rally;
        //         }));
        //
        //         this.pushHistory({
        //             lastUpdate: lastUpdate,
        //             desc_len: lastDescriptionLength,
        //             query: {
        //                 type: 'LimitIndex',
        //                 text: 'I only need serving tactics.' + (randomInt(2) ? 'I don\'t need other tactics.' : ''),
        //                 params: {
        //                     min: 1,
        //                     max: 3,
        //                     ...(randomInt(2) && {
        //                         other: 'abc',
        //                         test: '12345',
        //                     })
        //                 }
        //             },
        //             tactics,
        //             sequences,
        //         });
        //     }
        // }

        resolve();
    });
}
