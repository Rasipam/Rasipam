export const SortTypes = {
    ImportanceDown: 'ImportanceDown',
    ImportanceUp: 'ImportanceUp',
    UsageDown: 'UsageDown',
    UsageUp: 'UsageUp',
    WinRateDown: 'WinRateDown',
    WinRateUp: 'WinRateUp',
}

const tacticSorter = {
    [SortTypes.ImportanceDown]: (t1, t2) => t2.stat.importance - t1.stat.importance,
    [SortTypes.ImportanceUp]: (t1, t2) => t1.stat.importance - t2.stat.importance,
    [SortTypes.UsageDown]: (t1, t2) => t2.usage_count - t1.usage_count,
    [SortTypes.UsageUp]: (t1, t2) => t1.usage_count - t2.usage_count,
    [SortTypes.WinRateDown]: (t1, t2) => t2.stat.winRate0 - t1.stat.winRate0,
    [SortTypes.WinRateUp]: (t1, t2) => t1.stat.winRate0 - t2.stat.winRate0,
}

export default tacticSorter;
