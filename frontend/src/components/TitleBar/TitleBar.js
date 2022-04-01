import React from 'react';
import {inject, observer, useLocalObservable} from "mobx-react";
import {AppBar, Box, Button, Toolbar, Typography} from "@mui/material";
import {useTranslation} from "react-i18next";
import strings from "../../static/strings";
import TitleBarSelect from "./TitleBarSelect";
import {Check, Close} from "@mui/icons-material";
import Store from "../../store/store";
import {playerColors} from "../../static/theme";

function TitleBar({data, analysis}) {
    const {t} = useTranslation();
    const cache = useLocalObservable(() => ({
        cacheDataset: null,
        get dataset() {
            return this.cacheDataset || analysis.dataset;
        },
        cachePlayer: null,
        get player() {
            return this.cachePlayer || analysis.player;
        },
        cacheOpponents: null,
        get opponents() {
            return this.cacheOpponents || analysis.opponents;
        },
        get isDisabled() {
            return !this.cacheDataset && !this.cachePlayer && !this.cacheOpponents;
        },
        get isApplicable() {
            return this.sequenceCount > 0;
        },
        apply() {
            if (this.cacheDataset) analysis.setDataset(this.cacheDataset);
            if (this.cachePlayer) analysis.setPlayer(this.cachePlayer);
            if (this.cacheOpponents) analysis.setOpponents(this.cacheOpponents);
            if (this.cacheDataset || this.cachePlayer || this.cacheOpponents)
                analysis.restart();
            this.cacheDataset = null;
            this.cachePlayer = null;
            this.cacheOpponents = null;
        },
        clear() {
            this.cacheDataset = null;
            this.cachePlayer = null;
            this.cacheOpponents = null;
        },
        setDataset(ds) {
            this.setOpponents([]);
            this.setPlayer('');

            this.cacheDataset = (ds === analysis.dataset) ? null : ds;
        },
        setPlayer(p) {
            let ops = this.cacheOpponents;
            if (!ops) ops = [];
            if (ops.includes(p)) ops = ops.filter(op => op !== p);
            this.setOpponents(ops);

            this.cachePlayer = (p === analysis.player) ? null : p;
        },
        setOpponents(ops) {
            if (ops.length !== analysis.opponents.length) {
                this.cacheOpponents = ops;
                return;
            }
            for (const op of ops)
                if (!analysis.opponents.includes(op)) {
                    this.cacheOpponents = ops;
                    return;
                }
            this.cacheOpponents = null;
        },
        get players() {
            const dataStore = Store.getStores().data;
            return dataStore.getPlayers(this.dataset);
        },
        get availableOpponents() {
            const dataStore = Store.getStores().data;
            return dataStore.getPlayers(this.dataset)
                .filter(player => player !== this.player);
        },
        get sequenceCount() {
            const dataStore = Store.getStores().data;
            return dataStore.getMatches(this.dataset, this.player, this.opponents)
                .reduce((p, c) => c.sequenceCount + p, 0);
        },
    }));

    return <AppBar sx={{height: 40, boxShadow: 'none'}}>
        <Toolbar sx={{height: 40, minHeight: '0 !important'}}>
            <Typography variant={'h5'}
                        sx={{flexGrow: 1}}>
                {t(strings.SystemName)}
            </Typography>
            <TitleBarSelect label={t(strings.Dataset)}
                            options={data.datasets.map(ds => ({
                                value: ds.name,
                                label: t(strings.DatasetName, ds.name)
                            }))}
                            value={cache.dataset}
                            onChange={e => cache.setDataset(e.target.value)}/>
            <Box mr={3}/>
            <Box width={20} height={20} borderRadius={0.5} mr={0.5} backgroundColor={playerColors[0]}/>
            <TitleBarSelect label={t(strings.Player)}
                            options={cache.players.map(p => ({
                                value: p,
                                label: t(strings.PlayerName, p)
                            }))}
                            value={cache.player}
                            onChange={e => cache.setPlayer(e.target.value)}/>
            <Box mr={3}/>
            <Box width={20} height={20} borderRadius={0.5} mr={0.5} backgroundColor={playerColors[1]}/>
            <TitleBarSelect label={t(strings.Opponents)}
                            multiple
                            options={cache.availableOpponents.map(p => ({
                                value: p,
                                label: t(strings.OpponentName, p)
                            }))}
                            value={cache.opponents}
                            onChange={e => cache.setOpponents(e.target.value)}/>
            <Box mr={3}/>
            <Typography>{t(strings.SeqCount)}: {cache.sequenceCount}</Typography>
            <Box mr={3}/>
            {!cache.isDisabled && <React.Fragment>
                <Button disabled={!cache.isApplicable}
                        color={'success'}
                        sx={{minWidth: 0}}
                        onClick={cache.apply}>
                    <Check/>
                </Button>
                <Button color={'error'}
                        sx={{minWidth: 0}}
                        onClick={cache.clear}>
                    <Close/>
                </Button>
            </React.Fragment>}
        </Toolbar>
    </AppBar>
}

export default inject('data', 'analysis')(observer(TitleBar));
