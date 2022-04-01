import {inject, observer} from "mobx-react";
import SymbolSet from "./SymbolSets";
import {playerColors} from "../../static/theme";
import {useTheme} from "@mui/styles";
import uuid4 from "../../utils/uuid4";
import {Tooltip} from "@mui/material";

const AttrStat = inject('analysis')(({attrKey, values, analysis, width, size, padding, color}) => {
    const theme = useTheme();
    const bgColor = theme.palette.background.default;
    const fgColor = color;
    const maxBarWidth = width - size - padding * 7;
    const barHeight = size * 0.6;
    const rectStyle = {
        x: 0,
        y: -barHeight / 2,
        height: barHeight,
        strokeWidth: 0,
    }
    const textStyle = {
        x: padding,
        textAnchor: 'left',
        dominantBaseline: 'middle',
        strokeWidth: 0,
        fontSize: barHeight * 0.7,
    }

    const sum = Object.values(values).reduce((p, c) => p + c, 0);
    const showValues = Object.entries(values)
        .map(v => [v[0], v[1] / sum])
        .sort((v1, v2) => v2[1] - v1[1])
        .slice(0, 2)
    const symbolSet = SymbolSet.dataset(analysis.dataset);
    const renderer = symbolSet.attr(attrKey).size(size);
    return showValues.map(([val, ratio], i) => {
        const label = (ratio * 100).toFixed(1) + '%';
        const clipPathId = uuid4();
        return <g key={i}
                  transform={`translate(${padding}, ${(size + padding) * i})`}>
            <Tooltip title={val} placement={"left-end"}>
                <g transform={`translate(${size / 2 + padding}, ${size / 2})`}>
                    <rect x={-size / 2} y={-size / 2}
                          width={size} height={size}
                          fill={theme.palette.background.paper}
                          strokeWidth={0}/>
                    {renderer.render(val, false)}
                </g>
            </Tooltip>
            <g transform={`translate(${size + padding * 3}, ${size / 2})`}>
                <rect {...rectStyle} width={maxBarWidth} fill={bgColor}/>
                <text {...textStyle} fill={theme.palette.getContrastText(bgColor)}>{label}</text>
                <rect {...rectStyle} width={maxBarWidth * ratio} fill={fgColor}/>
                <defs>
                    <clipPath id={clipPathId}>
                        <rect {...rectStyle} width={maxBarWidth * ratio}/>
                    </clipPath>
                </defs>
                <text {...textStyle} clipPath={`url(#${clipPathId})`}
                      fill={theme.palette.getContrastText(fgColor)}>{label}</text>
            </g>
        </g>
    });
})

function HitStat({hitAdditional, analysis, width, height, player, size, padding, visibleAttr, shrink=false}) {
    const attrs = analysis.attrs;
    return hitAdditional
        .map((values, aId) => [values, attrs[aId]])
        .filter(([values], aId) => visibleAttr.has(aId))
        .map(([values, attrKey], aId) => [values, attrKey, aId])
        .filter(([values]) => values !== null)
        .map(([values, attrKey, originalAId], aId) => {
            return <g transform={`translate(0, ${(size * 2 + padding[0] + padding[1]) * (shrink ? aId : originalAId)})`}
                      fill={playerColors[player]}
                      stroke={playerColors[player]}>
                <AttrStat attrKey={attrKey}
                          values={values}
                          size={size}
                          padding={padding[0]}
                          color={playerColors[player]}
                          width={width}
                />
            </g>
        });
}

export default inject('analysis')(observer(HitStat));
