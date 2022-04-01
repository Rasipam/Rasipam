import React from 'react';
import {inject, observer} from "mobx-react";
import SymbolSet from "./SymbolSets";
import {playerColors} from "../../static/theme";
import {Tooltip} from "@mui/material";
import {freqRate2color} from "../../utils/winRate2color";

function Glyph({hit, hitAdditional, size, analysis, freq, player}) {
    const glyphSet = SymbolSet.dataset(analysis.dataset);
    const values = analysis.attrs.map((attrKey, aId) => {
        let value = hit[aId], opacity = 1;
        if (!value) {
            const optVal = Object.entries(hitAdditional[aId]).sort((v1, v2) => v2[1] - v1[1]);
            value = optVal[0][0];
            opacity = optVal[0][1] / freq;
        }
        return [value, opacity, attrKey];
    })
    return <Tooltip placement={"top"}
                    title={<table>
                        <tbody>
                        {values.map(v => <tr key={v[2]}>
                            <th>{v[2]}</th>
                            <td>{v[0]}</td>
                        </tr>)}
                        </tbody>
                    </table>}>
        <g>
            <rect x={-size / 2} y={-size / 2} width={size} height={size}
                // strokeWidth={1} stroke={'black'}
                  fill={'white'}/>
            {analysis.attrs.map((attrKey, aId) => {
                const renderer = glyphSet
                    .attr(attrKey)
                    .size(size);
                const [value, opacity] = values[aId];
                return <g key={attrKey}
                          opacity={freqRate2color(opacity)}
                          stroke={playerColors[player]}
                          fill={playerColors[player]}>
                    {renderer.render(value, true)}
                </g>
            })}
        </g>
    </Tooltip>
}

export default inject('analysis')(observer(Glyph));
