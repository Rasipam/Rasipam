const insert = (distribution, idx, win) => {
    while (distribution.length <= idx + 1) distribution.push([0, 0]);
    distribution[idx][win ? 0 : 1] += 1;
    return distribution[idx];
}

export default function tacticDistributionStat(rallies) {
    const distribution = [];
    let maxX = 0, maxY = 0;
    rallies.forEach(rally => {
        console.log('Rally', JSON.parse(JSON.stringify(rally)))
        const bar = insert(distribution, rally.index, rally.win)
        maxX = Math.max(distribution.length, maxX);
        maxY = Math.max(bar[0] + bar[1], maxY);
    });
    return {
        stat: {
            maxX,
            maxY,
        },
        distribution,
    };
}
