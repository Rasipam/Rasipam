/**
 * Created by Jiang Wu on 2022/2/15
 */

import {memo} from 'react';
import {Typography} from "@mui/material";
import styled from "@emotion/styled";

function Point({
                   id,
                   t,
                   cx,
                   cy,
                   r,
                   color,
                   isHovered,
                   isSelected,
                   isFavorite,
                   isCache,
                   onSelect,
               }) {
    return <Pos style={{transform: `translate(${cx}px, ${cy}px)`}}>
        <P style={{
            width: r * 2,
            height: r * 2,
            borderRadius: `${r}px`,
            backgroundColor: color,
            lineHeight: `${r * 2}px`,
            transform: `translate(-${r}px, -${r}px)`,
        }}>
            <Typography variant={'caption'}>{id}</Typography>
        </P>
    </Pos>;
}

const Pos = styled('div')({
    position: 'absolute',
    width: 0,
    height: 0,
    overflow: 'visible',
});

const P = styled('div')({
    fontSize: '0.75rem',
    fontWeight: 400,
    letterSpacing: '0.03333em',
    textAlign: 'center',
    cursor: 'pointer',
    '&:hover': {
        outline: '-webkit-focus-ring-color auto 1px'
    }
})

export default memo(Point);
