export const FilterTypes = {
    usage: 'usage',
    winRate: 'winRate',
    importance: 'importance',
}

export const tacticFilter = {
    [FilterTypes.usage]: (t1, t2) => t2.usage_count - t1.usage_count,
}