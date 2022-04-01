import React, {memo} from 'react';
import Glyph from "./Glyph";
import HitStat from "./HitStat";

function Hit({
                 size,
                 maxHeight,
                 hit,
                 hitAdditional,
                 open,
                 player,
                 freq,
                 statSize,
                 statPadding,
                 statShrink,
                 visibleStatAttr
             }) {
    return <React.Fragment>
        {/* itself */}
        <g transform={`translate(${size / 2}, ${size / 2})`}>
            <Glyph hit={hit}
                   hitAdditional={hitAdditional}
                   freq={freq}
                   player={player}
                   size={size * 0.8}/>
        </g>
        {/* detail */}
        {open && <g transform={`translate(0, ${size})`}>
            <HitStat hitAdditional={hitAdditional}
                     freq={freq}
                     width={size}
                     player={player}
                     height={maxHeight - size}
                     size={statSize}
                     shrink={statShrink}
                     padding={statPadding}
                     visibleAttr={visibleStatAttr}/>
        </g>}
    </React.Fragment>
}

export default memo(Hit);
