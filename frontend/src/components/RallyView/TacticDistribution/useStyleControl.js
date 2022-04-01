import {useMemo} from "react";

const defaultSteps = [1, 2, 5, 10, 20, 30, 40, 50, 100];
const defaultCounts = [3, 5];

function stepwise(val, availableSteps, countLimit) {
    const steps = availableSteps || defaultSteps,
        counts = countLimit || defaultCounts;

    const results = [];

    for (const step of steps) {
        const count = Math.ceil(val / step);
        if (count < counts[0]) {
            results.push([step, count, step * count - val + (counts[0] - count) * 20 + 1000]);
        } else if (count > counts[1]) {
            results.push([step, count, step * count - val + (count - counts[1]) * 20 + 1000]);
        } else {
            results.push([step, count, step * count - val]);
        }
    }
    results.sort((r1, r2) => r1[2] - r2[2]);
    return results[0].slice(0, 2);
}

export default function useStyleControl({width, height}, {maxX, maxY}) {
    const [step, count] = stepwise(maxY);

    return useMemo(() => {
        const fontSize = 16;
        const textPadding = 3;
        const pt = 15;
        const pr = 15;
        const sideBarWidth = (step * count).toString().length * fontSize / 2 + textPadding * 2;
        const unitWidth = Math.max((width - sideBarWidth - pr) / maxX, 1);
        const bottomBarHeight = textPadding * 2 + fontSize;
        const unitHeight = (height - bottomBarHeight - pt) / (step * count);
        const barWidth = Math.min(20, unitWidth * 0.8);
        const arrowSize = [7, 7];
        return {
            width,
            height,
            sideBarWidth,
            unitWidth,
            textPadding,
            bottomBarHeight,
            fontSize: fontSize,
            unitHeight,
            ox: sideBarWidth,
            oy: height - bottomBarHeight,
            barWidth,
            barStartX: idx => sideBarWidth + unitWidth * idx + unitWidth / 2,
            hintLines: [...new Array(count)].map((_, i) => (i + 1) * step),
            arrowSize,
        }
    }, [step, count, width, maxX, height]);
}
