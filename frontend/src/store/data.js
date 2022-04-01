import {makeAutoObservable} from "mobx";
import Store from "./store";
import api from "./api";

export default class DataStore {
    datasets = []
    setDatasets = (newDatasets) => {
        this.datasets = newDatasets;
    }
    getPlayers = (datasetName) => {
        const players = [];
        this.datasets.forEach(ds => {
            if (ds.name !== datasetName) return;

            ds.matches.forEach(match => {
                match.players.forEach(player => {
                    if (!players.includes(player[0]))
                        players.push(player[0]);
                })
            })
        })
        return players;
    }
    getMatches = (datasetName, player, opponents) => {
        const matches = [];
        this.datasets.forEach(ds => {
            if (ds.name !== datasetName) return;

            ds.matches.forEach(match => {
                const idx = match.players.map(p => p[0]).indexOf(player);
                if (idx === -1) return;

                if (opponents.includes(match.players[1 - idx][0]))
                    matches.push(match);
            })
        })
        return matches;
    }

    getVideoSrc = videoName =>
        (videoName && videoName.endsWith('mp4')) ?
            api.videoSrc(videoName) :
            null;
    processText = api.processText;
    processAudio = api.processAudio;

    constructor() {
        makeAutoObservable(this);
        Store.register('data', this);
    }

    init = () => new Promise((resolve, reject) => {
        api.getDatasets()
            .then(this.setDatasets)
            .then(resolve)
    })
}
