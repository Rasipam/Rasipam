import {Box, Checkbox, FormControlLabel, IconButton, TableRow} from "@mui/material";
import {ArrowDownward, ArrowUpward, Favorite, FavoriteBorder, Remove} from "@mui/icons-material";
import {BodyCell} from "./TableCell";
import {winColors} from "../../static/theme";
import Bar from "./Bar";
import TacticDetail from "./TacticDetail";
import React, {useState} from "react";

const barType = 'Overlap';
// const barType = 'Left';
// const barType = 'Top';
const barHeight = 30;
const showBorder = false;
const doubleAlign = 'left';
// const doubleAlign = 'center';
const trackColor = 'rgb(244,244,244)';
// const trackColor = 'background.paper';

function PreviewingLabel({oldIndex, newIndex}) {
    const captionStyle = {
        style: {
            fontSize: '0.9rem',
            lineHeight: 2,
            letterSpacing: '0.033333em',
            fontWeight: 400,
            verticalAlign: 'middle',
            textAlign: 'right',
        }
    }
    return <table style={{flex: '0 0 auto', height: 'fit-content'}}>
        <tbody>
        <tr>
            {oldIndex !== -1 ? <td>{oldIndex + 1}</td> : <td>&nbsp;&nbsp;</td>}
            <td>
                <Box component={'span'}
                     sx={{
                         color: oldIndex === -1 || newIndex === -1 ? 'text.default' : (newIndex < oldIndex ? 'success.main' :
                             (newIndex === oldIndex ? 'text.default' :
                                 'error.main'))
                     }}>
                    {oldIndex !== -1 && newIndex !== -1 && newIndex < oldIndex && <ArrowUpward {...captionStyle}/>}
                    {(oldIndex === -1 || newIndex === -1 || newIndex === oldIndex) && <Remove {...captionStyle}/>}
                    {oldIndex !== -1 && newIndex !== -1 && newIndex > oldIndex && <ArrowDownward {...captionStyle}/>}
                </Box>
            </td>
            {newIndex !== -1 ? <td>{newIndex + 1}</td> : <td>&nbsp;&nbsp;</td>}
        </tr>
        </tbody>
    </table>
}

export function Tactic({tactic, tId, selected, onSelect, favorite, onFavorite, isPreviewing, highlightBackground, noSelect}) {
    const [viewDetail, setViewDetail] = useState(false);

    const handleSelect = e => {
        e.stopPropagation();
        onSelect(!selected);
    }

    const handleFavorite = e => {
        e.stopPropagation();
        onFavorite(!favorite)
    }

    return <TableRow
        onClick={() => setViewDetail(a => !a)}
    >
        <BodyCell highlightBackground={highlightBackground}>
            {!!isPreviewing ? (!!noSelect ? <FormControlLabel checked={selected}
                              onClick={handleSelect}
                              control={<Checkbox/>}
                              label={<PreviewingLabel oldIndex={tactic.sortedIndex} newIndex={tactic.newSortedIndex}/>}
                              disabled
            /> : <FormControlLabel checked={selected}
                                   onClick={handleSelect}
                                   control={<Checkbox/>}
                                   label={<PreviewingLabel oldIndex={tactic.sortedIndex} newIndex={tactic.newSortedIndex}/>}
            />) : <FormControlLabel checked={selected}
                                    onClick={handleSelect}
                                    control={<Checkbox/>}
                                    label={tId + 1}/>}
        </BodyCell>
        <BodyCell highlightBackground={highlightBackground}>
            <TacticDetail open={viewDetail}
                          tactic={tactic.tactic}
                          user={tactic.user}
                          tacticAddition={tactic.tactic_surrounding}
                          usageCount={tactic.usage_count}/>
        </BodyCell>
        <BodyCell highlightBackground={highlightBackground}>
            <Bar variant={`Single${barType}`}
                 height={barHeight}
                 showBorder={showBorder}
                 textWidth={26}
                 trackColor={trackColor}
                 value={tactic.usage_count / tactic.globalStat.usage}
                 label={tactic.usage_count}/>
        </BodyCell>
        <BodyCell highlightBackground={highlightBackground}>
            {
                doubleAlign !== 'center' ?
                    <Bar variant={`Single${barType}`}
                         height={barHeight}
                         showBorder={false}
                         value={tactic.stat.winRate0}
                         textWidth={41}
                         barColor={winColors[0]}
                         trackColor={winColors[1]}
                         label={`${(tactic.stat.winRate0 * 100).toFixed(1)}%`}/> :
                    <Bar variant={`Double${barType}`}
                         height={barHeight}
                         showBorder={false}
                         value={[tactic.stat.winRate0, tactic.stat.winRate1]}
                         barColor={winColors}
                         trackColor={trackColor}
                         doubleAlign={doubleAlign}
                         label={[
                             `${(tactic.stat.winRate0 * 100).toFixed(1)}%`,
                             `${(tactic.stat.winRate1 * 100).toFixed(1)}%`
                         ]}/>
            }
        </BodyCell>
        <BodyCell highlightBackground={highlightBackground}>
            <Bar variant={`Single${barType}`}
                 height={barHeight}
                 showBorder={showBorder}
                 textWidth={34}
                 trackColor={trackColor}
                 value={tactic.stat.importance / tactic.globalStat.importance}
                 label={tactic.stat.importance}/>
        </BodyCell>
        <BodyCell highlightBackground={highlightBackground}>
            <IconButton onClick={handleFavorite}>
                {favorite ? <Favorite color={'error'}/> : <FavoriteBorder/>}
            </IconButton>
        </BodyCell>
    </TableRow>
}
