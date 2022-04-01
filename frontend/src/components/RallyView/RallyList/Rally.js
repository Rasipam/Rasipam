import React, {memo, useRef} from 'react';
import {inject, observer} from "mobx-react";
import {IconButton, Tooltip, Typography} from "@mui/material";
import {PlayArrow, Visibility} from "@mui/icons-material";
import useRallyHeight from "./useRallyHeight";
import {useTheme} from "@mui/styles";
import {playerColors, transition} from "../../../static/theme";
import useClick from "../../../utils/useClick";
import styled from "@emotion/styled";
import {useHover} from "ahooks";

const actionProps = ({
                         visible = false, highlight = false
                     }) => ({
    className: 'action',
    size: 'small',
    sx: {
        transition: 'opacity .1s linear',
        opacity: (highlight || visible) ? 1 : 0,
        color: highlight ? 'error.main' : 'text.primaryText',
    }
})

function Rally({rally, isPlaying, onPlay, isExpand, onExpand, analysis}) {
    const theme = useTheme();

    const rootRef = useRef(null);
    const visible = useHover(rootRef);

    const handleViewDetail = e => {
        e.stopPropagation();
        onExpand(rally);
    }

    const handlePlay = e => {
        e.stopPropagation();
        onPlay(rally);
    }

    const {handleClick, handleDoubleClick} = useClick({
        onClick: handleViewDetail,
        onDoubleClick: handlePlay,
    });

    const {rallyHeight, headHeight, attrHeight, m} = useRallyHeight(analysis.attrs.length);
    const rh = rallyHeight(isExpand), mh = m(isExpand);

    // const cellMargin = isExpand ? 100 : theme.spacing(1);
    const hitWidth = isTactic => isTactic ? 21 : 15;
    const cellMargin = isExpand ? 100 : headHeight * 2;
    const actionWidth = isExpand ? 100 : (visible ? (headHeight * 2) : theme.spacing(1));
    const cellWidth = isExpand ? 100 : hitWidth(true) + parseInt(theme.spacing(1));

    return <Container ref={rootRef}
                      style={{
                          height: rh,
                      }}>
        <Tb style={{margin: [mh, 0]}}>
            <Tr style={{height: headHeight}}
                onClick={handleClick}
                onDoubleClick={handleDoubleClick}>
                <Th style={{maxWidth: actionWidth}}>
                    <IconButton {...actionProps({visible, highlight: isExpand})}
                                onClick={handleViewDetail}>
                        <Visibility/>
                    </IconButton>
                    <IconButton {...actionProps({visible, highlight: isPlaying})}
                                onClick={handlePlay}>
                        <PlayArrow/>
                    </IconButton>
                </Th>

                {rally.rally.map((hit, hId) => {
                    const isTactic = (rally.tacticPos[0] <= hId) && (hId < rally.tacticPos[1]);
                    const size = hitWidth(isTactic);
                    const color = playerColors[(hId + (rally.is_server ? 0 : 1)) % 2]
                    const bgcolor = isTactic ? color : theme.palette.background.paper;
                    return <Td key={hId}
                        style={{
                            maxWidth: cellWidth,
                            marginLeft: (hId === 0) && cellMargin,
                        }}>
                        <Hit style={{
                            backgroundColor: bgcolor,
                            color: theme.palette.getContrastText(bgcolor),
                            borderColor: color,
                            width: size,
                            height: size,
                            lineHeight: `${size}px`,
                            fontSize: `${size * 0.75}px`,
                        }}>{hId + 1}</Hit>
                    </Td>
                })}
            </Tr>
            {analysis.attrs.map((attr, aId) => (
                <Tr key={aId}
                    style={{height: attrHeight}}>
                    <Th style={{maxWidth: actionWidth}}>
                        <Typography variant={'body2'} fontWeight={"bold"}>{attr}</Typography>
                    </Th>

                    {rally.rally.map((hit, hId) => (
                        <Td key={hId}
                            style={{
                                maxWidth: cellWidth,
                                borderTop: `1px solid ${theme.palette.background.default}`,
                                marginLeft: (hId === 0) && cellMargin,
                            }}>
                            <Tooltip title={hit[aId]}>
                                <Typography variant={'body2'} noWrap>{hit[aId]}</Typography>
                            </Tooltip>
                        </Td>
                    ))}
                </Tr>
            ))}
            {isExpand && <Head style={{
                top: mh,
                height: rh - mh * 4,
                left: actionWidth
            }}/>}
        </Tb>
    </Container>
}

const Container = styled('div')(({theme}) => ({
    transition: transition('height'),
    borderBottom: '1px solid',
    borderBottomColor: theme.palette.background.default,
    overflow: 'hidden',
    position: 'relative',
}))
const Head = styled('div')(({theme}) => ({
    position: 'absolute',
    left: 100,
    transition: transition('left'),
    width: 0,
    borderRight: `1px solid ${theme.palette.background.default}`,
}));
const Tb = styled('div')({
    height: 'fit-content',
    overflow: 'auto hidden',
})
const Tr = styled('div')({
    display: 'flex',
    flexWrap: 'nowrap',
})
const Td = styled('div')(({theme}) => ({
    transition: transition('all'),
    padding: theme.spacing(0, 0.5),
    flex: '0 0 auto',
    display: 'flex',
    justifyContent: 'center',
    alignItems: 'center',
    whiteSpace: 'nowrap',
    width: 100,
}))
const Th = styled('div')(({theme}) => ({
    position: 'absolute',
    background: theme.palette.background.paper,
    transition: transition('all'),
    padding: theme.spacing(0, 0.5),
    flex: '0 0 auto',
    display: 'flex',
    justifyContent: 'center',
    alignItems: 'center',
    whiteSpace: 'nowrap',
    width: 100,
}));
const Hit = styled('div')({
    borderRadius: '50%',
    border: '1px solid',
    textAlign: 'center',
})


export default memo(inject('analysis')(observer(Rally)));
