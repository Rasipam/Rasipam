import {inject, observer} from "mobx-react";
import Toolbar from "../Common/Toolbar";
import {useTranslation} from "react-i18next";
import strings from "../../static/strings";
import styled from "@emotion/styled";
import {winColors} from "../../static/theme";
import {freqRate2color} from "../../utils/winRate2color";

const Rect = styled('div')({
    width: 10,
    height: 10,
})

function TacticViewToolbar({}) {
    const {t} = useTranslation();
    return <Toolbar elements={[
        'Freq.%: ',
        <Rect style={{backgroundColor: freqRate2color(.1, 0)}}/>,
        <Rect style={{backgroundColor: freqRate2color(.1, 1)}}/>,
        '< 20%',
        <Rect style={{backgroundColor: freqRate2color(.4, 0)}}/>,
        <Rect style={{backgroundColor: freqRate2color(.4, 1)}}/>,
        '20% ~ 50%',
        <Rect style={{backgroundColor: freqRate2color(.8, 0)}}/>,
        <Rect style={{backgroundColor: freqRate2color(.8, 1)}}/>,
        '50% ~ 99%',
        <Rect style={{backgroundColor: freqRate2color(1, 0)}}/>,
        <Rect style={{backgroundColor: freqRate2color(1, 1)}}/>,
        '100%',
        <div style={{width: 10}}/>,
        <Rect style={{backgroundColor: winColors[0]}}/>,
        t(strings.PlayerWins),
        <Rect style={{backgroundColor: winColors[1]}}/>,
        t(strings.OpponentsWins),
    ]}/>;
};

export default inject()(observer(TacticViewToolbar));