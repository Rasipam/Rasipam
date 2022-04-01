import React, {useCallback, useRef, useState} from 'react';
import {inject, observer} from "mobx-react";
import {Box, Typography} from "@mui/material";
import Rally from "./Rally";
import {useVirtualList} from "ahooks";
import useRallyHeight from "./useRallyHeight";

function RallyList({rallies, title, onPlay, currentPlay, analysis}) {

    const [currentExpand, setCurrentExpand] = useState(null);
    const handleExpand = useCallback(rally => setCurrentExpand(oldId => (rally.id === oldId) ? null : rally.id), []);

    const containerRef = useRef(null);
    const wrapperRef = useRef(null);
    const {rallyHeight} = useRallyHeight(analysis.attrs.length);
    const [list] = useVirtualList(rallies, {
        containerTarget: containerRef,
        wrapperTarget: wrapperRef,
        itemHeight: i => rallyHeight(rallies[i].id === currentExpand),
        overscan: 5,
    });

    return <Box display={'flex'} flexDirection={'column'}
                height={'100%'}>
        <Typography sx={{flex: '0 0 auto'}}>{title}: {rallies.length}</Typography>
        <Box flex={1} overflow={'hidden'}>
            <div ref={containerRef} style={{height: '100%', overflow: 'hidden auto'}}>
                <div ref={wrapperRef}>
                    {list.map(({index, data: rally}) => <Rally key={rally.id}
                                                               rally={rally}
                                                               isExpand={currentExpand === rally.id}
                                                               onExpand={handleExpand}
                                                               isPlaying={currentPlay === rally.id}
                                                               onPlay={onPlay}/>)}
                </div>
            </div>
        </Box>
    </Box>
}

export default inject('analysis')(observer(RallyList));
