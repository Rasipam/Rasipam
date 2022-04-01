/**
 * Created by Jiang Wu on 2022/1/27
 */
import React, {useMemo, useRef} from "react";
import {Box, Typography} from "@mui/material";
import {useTheme} from "@mui/styles";
import {useSize} from "../../utils/useSize";

function overlap(p1, l1, p2, l2) {
    return p1 < p2 + l2 && p2 < p1 + l1;
}

function canStartFrom(startPoint, boxSize, grids, gridSize) {
    // out of boundary
    for (let axis of [0, 1])
        if (startPoint[axis] + boxSize[axis] > gridSize[axis]) return false;
    // overlap with previous grids
    for (const grid of grids)
        if (overlap(startPoint[0], boxSize[0], grid.x, grid.width) &&
            overlap(startPoint[1], boxSize[1], grid.y, grid.height))
            return false;
    return true;
}

function isPointInGrid(p, grid) {
    return grid.x <= p[0] && grid.x + grid.width > p[0] &&
        grid.y <= p[1] && grid.y + grid.height > p[1];
}

function getPotentialStartPoints(grids, gridSize) {
    const potentialStartPoints = [[0, 0]];
    grids.forEach(grid => {
        // delete
        for (let i = potentialStartPoints.length; i--;) {
            const p = potentialStartPoints[i];
            if (isPointInGrid(p, grid))
                potentialStartPoints.splice(i, 1);
        }
        // add new
        const newPoints = [
            [grid.x, grid.y + grid.height],
            [grid.x + grid.width, grid.y]
        ]
        newPoints.forEach(p => {
            if (p[0] >= gridSize[0] || p[1] >= gridSize[1]) return;
            for (const grid of grids)
                if (isPointInGrid(p, grid)) return;
            potentialStartPoints.push(p);
        });
    })
    potentialStartPoints.sort((p1, p2) => (p1[1] - p2[1]) || (p1[0] - p2[0]))
    return potentialStartPoints;
}

const useGridLayout = (gridSize, panels) => {
    return useMemo(() => {
        return panels.reduce((grids, {title, size: boxSize}) => {
            const potentialStartPoints = getPotentialStartPoints(grids, gridSize);
            for (const startPoint of potentialStartPoints) {
                if (!canStartFrom(startPoint, boxSize, grids, gridSize)) continue;

                const grid = {
                    x: startPoint[0],
                    y: startPoint[1],
                    width: boxSize[0],
                    height: boxSize[1],
                };
                grids.push(grid);

                return grids;
            }
            console.warn(`Fail to insert panel ${title}!`);
            grids.push({x: -1, y: -1, width: 0, height: 0});
            return grids;
        }, []);
    }, [panels, gridSize]);
}

const useBoxes = (grids, layoutSize, gridSize, padding) => {
    const theme = useTheme();
    return useMemo(() => {
        const paddingPx = parseInt(theme.spacing(padding));
        const lSize = [layoutSize.width || paddingPx, layoutSize.height || paddingPx];
        const gridPx = [0, 1].map(axis => (lSize[axis] - paddingPx) / gridSize[axis] - paddingPx);
        return grids.map(grid => ({
            x: grid.x * (gridPx[0] + paddingPx) + paddingPx,
            y: grid.y * (gridPx[1] + paddingPx) + paddingPx,
            width: grid.width * (gridPx[0] + paddingPx) - paddingPx,
            height: grid.height * (gridPx[1] + paddingPx) - paddingPx,
        }))
    }, [grids, layoutSize, gridSize, padding])
}

function Panel({box, title, toolbar, view}) {
    const theme = useTheme();

    return <Box sx={{
        position: 'absolute',
        backgroundColor: theme.palette.background.paper,
        left: box.x,
        top: box.y,
        width: box.width,
        height: box.height,
        borderRadius: `${theme.shape.borderRadius}px`,
        overflow: 'hidden',
    }}>
        <Box style={{
            width: box.width,
            height: 30,
            display: 'flex',
        }}>
            <Box style={{
                flex: '0 0 auto',
                backgroundColor: theme.palette.primary.main,
                paddingLeft: theme.spacing(2),
                paddingRight: theme.spacing(2),
                borderRadius: `0 0 ${theme.shape.borderRadius}px 0`,
            }}>
                <Typography variant={'subtitle1'}
                            sx={{
                                color: theme.palette.primary.contrastText
                            }}>
                    {title}
                </Typography>
            </Box>
            <Box flex={'1 1 0%'}
                 overflow={'hidden'}>
                {toolbar}
            </Box>
        </Box>
        <Box width={box.width}
             height={box.height - 30}
             overflow={'hidden'}>
            {view}
        </Box>
    </Box>
}

function LayoutManager({
                           panels = [],
                           size = [12, 12],
                           padding = 1
                       }) {
    const grids = useGridLayout(size, panels);

    const containerRef = useRef(null);
    const containerSize = useSize(containerRef);
    const boxes = useBoxes(grids, containerSize, size, padding);

    const theme = useTheme();
    return <Box ref={containerRef}
                width={'100%'}
                height={'100%'}
                backgroundColor={theme.palette.background.default}
                position={'relative'}>
        {panels.map((panel, pId) => (
            <Panel key={pId}
                   box={boxes[pId]}
                   title={panel.title}
                   toolbar={panel.toolbar}
                   view={panel.view}/>)
        )}
    </Box>
}

export default React.memo(LayoutManager);
