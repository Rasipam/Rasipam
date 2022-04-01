import React from "react";

export default function HintLine({y, styleControl, color = 'red', bgColor = 'white'}) {
    const {height, bottomBarHeight, unitHeight, ox, width, sideBarWidth, fontSize, textPadding} = styleControl;
    const val = ((height - y - bottomBarHeight) / unitHeight).toFixed(0);
    if (val <= 0) return null;
    return <g transform={`translate(${ox}, ${y})`} pointerEvents={'none'}>
        <line x1={0} x2={width - ox}
              style={{stroke: color, strokeWidth: 1}}/>
        <rect x={-sideBarWidth} width={sideBarWidth - 1}
              y={-fontSize / 2 - textPadding} height={fontSize + 2 * textPadding}
              fill={bgColor}/>
        <text x={-3} y={0}
              textAnchor={'end'} alignmentBaseline={'middle'}
              style={{fill: color}}>
            {val}
        </text>
    </g>;
}
