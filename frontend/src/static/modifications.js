const types = [
    {
        objective: 'TacticSet',
        label: '战术集',
        types: [
            {
                type: 'LimitIndex',
                label: '指定位置的战术',
                params: [{ key: 'min', label: '拍序号最小值' }, { key: 'max', label: '拍序号最大' }],
                example: '发球段的战术；相持段的战术',
            },
            {
                type: 'LimitLength',
                label: '修改战术拍数',
                params: [{ key: 'min', label: '最小拍数' }, { key: 'max', label: '最大拍数' }, { key: 'offset', label: '拍数变化' }],
                example: '想要战术长一点',
            },
        ]
    },
    {
        objective: 'Attribute',
        label: '属性',
        types: [
            {
                type: 'SetExistence',
                label: '是否考虑属性',
                params: [{ key: 'exist', label: '是否考虑' }],
                example: '这个属性暂时不管它',
            },
            {
                type: 'SetImportance',
                label: '调整属性重要程度',
                params: [{ key: 'importance', label: '重要度变化' }],
                example: '多关注落点的变化',
            },
        ]
    },
    {
        objective: 'Tactic',
        label: '单个战术',
        types: [
            {
                type: 'Delete',
                label: '删除战术',
                params: [{ key: 'index', label: '战术编号' }],
                example: '这个战术没啥意思，不要了',
            },
            {
                type: 'Split',
                label: '细分笼统战术',
                params: [{ key: 'index', label: '战术编号' }, { key: 'attr', label: '针对属性细分' }, { key: 'value', label: '针对值细分' }],
                example: '对攻的战术应该注重落点的变化',
            },
            {
                type: 'Merge',
                label: '合并相似战术',
                params: [{ key: 'index', label: '战术编号' }],
                example: '这两个战术很相似，可以合并到一起',
            },
        ]
    },
    {
        objective: 'Hit',
        label: '单个战术中的拍',
        types: [
            {
                type: 'Increment',
                label: '扩展拍',
                params: [{ key: 'index', label: '战术编号' }, { key: 'direction', label: '扩展方向' }, { key: 'hitCount', label: '扩展拍数' }],
                example: '这个战术后面应该还有后续的执行',
            },
            {
                type: 'Decrement',
                label: '忽略拍',
                params: [{ key: 'index', label: '战术编号' }, { key: 'hits', label: '忽略的拍序号' }],
                example: '这个战术的第一拍击球和战术关系不大',
            },
        ]
    },
    {
        objective: 'Value',
        label: '单个战术中的值',
        types: [
            {
                type: 'Replace',
                label: '替换值',
                params: [{ key: 'index', label: '战术编号' }, { key: 'hit', label: '拍' }, { key: 'attr', label: '属性' }, { key: 'target', label: '目标值' }],
                example: '第三拍技术应该用弧圈比较多？',
            },
            {
                type: 'Ignore',
                label: '忽略值',
                params: [{ key: 'index', label: '战术编号' }, { key: 'hit', label: '拍' }, { key: 'attr', label: '属性' }],
                example: '第二拍的旋转不是战术的核心，不用管',
            },
            {
                type: 'Explore',
                label: '明确值',
                params: [{ key: 'index', label: '战术编号' }, { key: 'hit', label: '拍' }, { key: 'attr', label: '属性' }],
                example: '第一拍的技术需要细化',
            },
        ]
    }
]

export default types;