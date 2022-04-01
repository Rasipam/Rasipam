/**
 * Created by Jiang Wu on 2022/1/27
 */

import {inject, observer} from "mobx-react";
import {Box, Stack} from "@mui/material";
import TacticDistribution from "./TacticDistribution";
import RallyList from "./RallyList";
import VideoPlayer from "./VideoPlayer";
import {useCallback, useEffect, useState} from "react";
import {useTranslation} from "react-i18next";
import strings from "../../static/strings";

const defaultCurrentPlay = {
    id: null,
    videoName: null,
    startTime: null,
    endTime: null,
}

const RallyView = inject('analysis', 'data')(observer(({analysis, data}) => {
    const {t} = useTranslation();
    const rallies = analysis.ralliesOfSelectedTactics;

    const [currentPlay, setCurrentPlay] = useState(defaultCurrentPlay);
    const hasCurrentPlay = (currentPlay.id === null) || rallies.some(rally => rally.id === currentPlay.id);
    useEffect(() => {
        if (hasCurrentPlay) return;
        setCurrentPlay(defaultCurrentPlay)
    }, [hasCurrentPlay])
    const handlePlay = useCallback(rally => {
        setCurrentPlay({
            id: rally.id,
            videoName: rally.video_name,
            startTime: rally.start_time,
            endTime: rally.end_time,
        })
    }, []);
    const rallySort = (r1, r2) => r1.tacticPos[0] - r2.tacticPos[0];

    return <Stack width={'100%'} height={'100%'}
                  p={1}>
        <Box mb={1} flex={'0 0 150px'} overflow={'hidden'}>
            <TacticDistribution rallies={rallies}/>
        </Box>
        <Box mb={1} flex={'1 0 0%'} overflow={'hidden'}>
            <RallyList rallies={rallies.filter(rally => rally.win).sort(rallySort)}
                       currentPlay={currentPlay.id}
                       title={t(strings.PlayerWins)}
                       onPlay={handlePlay}/>
        </Box>
        <Box mb={1} flex={'1 0 0%'} overflow={'hidden'}>
            <RallyList rallies={rallies.filter(rally => !rally.win).sort(rallySort)}
                       currentPlay={currentPlay.id}
                       title={t(strings.OpponentsWins)}
                       onPlay={handlePlay}/>
        </Box>
        <Box flex={'0 0 auto'} overflow={'hidden'}>
            <VideoPlayer src={data.getVideoSrc(currentPlay.videoName)}
                         startTime={currentPlay.startTime}
                         endTime={currentPlay.endTime}/>
        </Box>
    </Stack>;
}));

const RallyViewToolbar = inject()(observer(({}) => {
    return null;
}));

export default function useRallyView() {
    return {
        view: <RallyView/>,
        toolbar: <RallyViewToolbar/>,
    }
}
