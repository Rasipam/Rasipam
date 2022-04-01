const dict = {
    SystemName: {cn: '交互式战术挖掘系统', en: 'IPAM'},
    Dataset: {cn: '数据集', en: 'Dataset'},
    Player: {cn: '球员', en: 'Player'},
    Opponents: {cn: '对手', en: 'Opponents'},
    SeqCount: {cn: '回合数', en: 'Sequence Count'},

    QueryView: {cn: '查询视图', en: 'Query View'},
    PreviewChange: {cn: '预览', en: 'Preview'},
    Undo: {cn: '撤销', en: 'Undo'},
    ApplyChange: {cn: '确认', en: 'Apply'},
    CancelChange: {cn: '取消', en: 'Cancel'},
    EmptyType: {cn: '选择修改类型', en: 'Select Query Type'},

    PreviewView: {cn: '预览视图', en: 'Preview View'},
    TacticView: {cn: '战术视图', en: 'Tactic View'},

    RallyView: {cn: '回合视图', en: 'Rally View'},
    PlayerWins: {cn: '得分', en: 'Player Wins'},
    OpponentsWins: {cn: '失分', en: 'Player Loses'},
    Frequency: {cn: '频次', en: 'Freq.'},
    Index: {cn: '拍次', en: 'Hit No.'},
    videoHintPending: {cn: '请选择回合播放视频', en: 'Please select a rally to play'},
    videoHintError: {cn: '视频资源错误，请联系网站维护者', en: 'The video resource cannot be played'},
    videoHintLoading: {cn: '视频正在加载', en: 'The video is loading ... Please wait ...'},

    ProjectView: {cn: '投影视图', en: 'Project View'},

    Error: {cn: '错误', en: 'Error'},
}

const words = Object.keys(dict);
const strings = Object.fromEntries(words.map(word => [word, word]));
export default strings;

export const languagePacks = {
    cn: { translation: Object.fromEntries(words.map(word => [word, dict[word].cn])) },
    en: { translation: Object.fromEntries(words.map(word => [word, dict[word].en])) },
};
