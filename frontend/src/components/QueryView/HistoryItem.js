/**
 * Created by Jiang Wu on 2022/2/14
 */

import React, {useCallback, useRef} from 'react';
import {alpha, Box, Chip, darken, Divider, lighten, Typography} from "@mui/material";
import {useTranslation} from "react-i18next";
import {inject, observer} from "mobx-react";
import {useTheme} from "@mui/styles";
import formatTime from "../../utils/formatTime";
import {ArrowDownward, ArrowUpward, Remove} from "@mui/icons-material";
import {useHover} from "ahooks";
import {transition} from "../../static/theme";
import {useSize} from "../../utils/useSize";

function Offset({val, offset, label}) {
    const captionStyle = {
        style: {
            fontSize: '0.75rem',
            lineHeight: 1.66,
            letterSpacing: '0.033333em',
            fontWeight: 400,
            verticalAlign: 'middle',
            textAlign: 'right',
        }
    }

    return <tr {...captionStyle}>
        <th>{label}:</th>
        <td>{val}</td>
        <td>
            <Box component={'span'}
                 ml={0.5}
                 sx={{
                     color: offset > 0 ? 'success.main' :
                         (offset === 0 ? 'text.default' :
                             'error.main')
                 }}>
                {offset > 0 && <ArrowUpward {...captionStyle}/>}
                {offset === 0 && <Remove {...captionStyle}/>}
                {offset < 0 && <ArrowDownward {...captionStyle}/>}
                {Math.abs(offset)}
            </Box>
        </td>
    </tr>
}

function QueryParams({type, params, tColor}) {
    const {t} = useTranslation();

    return <Box flex={1} maxWidth={'calc(100% - 120px)'}>
        <Typography color={tColor}>{t(type)}</Typography>
        <Box display={'flex'} flexWrap={'wrap'} overflow={'hidden'}>
            {Object.entries(params)
                .map(([key, val]) => (
                    <Chip key={key}
                          size={"small"}
                          label={`${key}: ${val}`}
                          sx={{
                              m: 0.5,
                              background: alpha(tColor, 0.18),
                              color: tColor,
                          }}/>
                ))}
        </Box>
    </Box>
}

function Eval({dl, dlOffset, tc, tcOffset}) {
    return <table style={{flex: '0 0 auto', height: 'fit-content'}}>
        <tbody>
        <Offset val={tc} label={'Tac. Num'} offset={tcOffset}/>
        <Offset val={dl} label={'Score'} offset={dlOffset}/>
        </tbody>
    </table>
}

function ItemBox({idx, type, text, params, selected, onSelect, time, dl, dlOffset, tc, tcOffset}) {
    const {t} = useTranslation();
    const theme = useTheme();
    const bgcolor = lighten(theme.palette.background.default, 0.6);
    const color = selected ? darken(bgcolor, 0) : bgcolor;
    const borderColor = selected ? darken(bgcolor, 0.7) : bgcolor;
    const tColor = theme.palette.getContrastText(color);
    const rootRef = useRef(null);
    const isHovered = useHover(rootRef);

    const showDetail = isHovered || selected;

    const headRef = useRef(null);
    const {height: headHeight} = useSize(headRef);
    const contentRef = useRef(null)
    const {height: contentHeight} = useSize(contentRef);

    return <Box ref={rootRef}
                bgcolor={color}
                borderRadius={1}
                mt={0} mr={1} mb={1} ml={1}
                pt={0.5} pr={1.5} pb={0.5} pl={1.5}
                onClick={onSelect}
                overflow={'hidden'}
                sx={{
                    borderWidth: `1px 1px 1px 5px`,
                    borderStyle: 'solid',
                    borderColor: color,
                    borderLeftColor: borderColor,
                    height: showDetail ? (headHeight + contentHeight + parseInt(theme.spacing(3))) : (headHeight + parseInt(theme.spacing(1))),
                    cursor: 'pointer',
                    transition: transition('all'),
                    '&:hover': {
                        bgcolor: darken(bgcolor, 0.1),
                        borderColor: darken(bgcolor, 0.1),
                        borderLeftColor: selected ? borderColor : darken(bgcolor, 0.1),
                    },
                }}>
        <Box ref={headRef}
             display={'flex'}
             justifyContent={'space-between'}>
            <Typography color={tColor} mr={1}>{text}</Typography>
            <Typography variant={'caption'} mt={'0.125rem'}>{formatTime(time)}</Typography>
        </Box>

        <Divider sx={{m: 0.5}}/>

        <Box ref={contentRef}
             display={'flex'}
             justifyContent={'space-between'}
             overflow={'hidden'}>
            <QueryParams type={type}
                         params={params || {}}
                         tColor={tColor}/>
            <Eval dl={dl} dlOffset={dlOffset} tc={tc} tcOffset={tcOffset}/>
        </Box>
    </Box>
}

function HistoryItem({query, analysis, idx, ...props}) {
    const handleSelect = useCallback(() => analysis.viewHistory(idx), [analysis, idx]);
    const itemBoxProps = {
        idx,
        params: query?.params,
        selected: analysis.currentViewHistory === idx,
        onSelect: handleSelect,
        ...props,
    }
    return (query === null) ?
        <ItemBox type={'Init'}
                 text={'Initially mine tactics.'}
                 {...itemBoxProps}/> :
        <ItemBox type={query.type}
                 text={query.text}
                 {...itemBoxProps}/>;
}

export default inject('analysis')(observer(HistoryItem));
