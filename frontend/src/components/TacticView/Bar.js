import {memo} from "react";
import {Box, Typography} from "@mui/material";
import {useTheme} from "@mui/styles";
import getJsonValFromDotChain from "../../utils/getJsonValFromDotChain";

const colorTypes = ['rgb', 'rgba', 'hsl', '#'];

function isNormalColor(color) {
    for (const key of colorTypes)
        if (color.startsWith(key)) return true;
    return false;
}

function getFontSize(height) {
    return `${Math.min(height * 0.04, 1)}rem`
}

function DoubleTopBar({
                          trackColor = 'background.paper',
                          barColor = ['primary.main', 'secondary.main'],
                          value,
                          label = [null, null],
                          height = 20,
                          showBorder = true,
                      }) {
    const val = value.map(v => (v * 100).toFixed(1));
    return <Box position={'relative'}
                display={'flex'}
                height={height}
                bgcolor={trackColor}
                borderLeft={`${showBorder ? 1 : 0}px solid ${barColor[0]}`}
                borderRight={`${showBorder ? 1 : 0}px solid ${barColor[1]}`}>
        <Typography position={'absolute'}
                    fontSize={getFontSize(height)}
                    bottom={height + 2}
                    left={0}
                    right={0}
                    textAlign={'center'}
                    height={height} lineHeight={`${height}px`}>
            {label[0] || val[0]}
        </Typography>
        <Box bgcolor={barColor[0]}
             height={height}
             ml={`${val[1] / 2}%`}
             width={`${val[0] / 2}%`}/>
        <Box bgcolor={barColor[1]}
             height={height}
             width={`${val[1] / 2}%`}/>
    </Box>;
}

function DoubleLeftBar({
                           trackColor = 'background.paper',
                           barColor = ['primary.main', 'secondary.main'],
                           value,
                           label = [null, null],
                           height = 20,
                           showBorder = true,
                       }) {
    const val = value.map(v => (v * 100).toFixed(1));
    return <Box width={'100%'}
                display={'flex'}
                alignItems={'center'}>
        <Typography width={40} flex={`0 0 40px`}
                    textAlign={"right"} pr={0.5}
                    fontSize={getFontSize(height)}
                    height={height} lineHeight={`${height}px`}>
            {label[0] || val[0]}
        </Typography>
        <Box flex={'1 1 0%'}
             overflow={'hidden'}
             display={'flex'}
             height={height}
             bgcolor={trackColor}
             borderLeft={`${showBorder ? 1 : 0}px solid ${barColor[0]}`}
             borderRight={`${showBorder ? 1 : 0}px solid ${barColor[0]}`}>
            <Box bgcolor={barColor[0]}
                 height={height}
                 ml={`${val[1] / 2}%`}
                 width={`${val[0] / 2}%`}/>
            <Box bgcolor={barColor[1]}
                 height={height}
                 width={`${val[1] / 2}%`}/>
        </Box>
    </Box>;
}

function DoubleOverlapBar({
                              trackColor = 'background.paper',
                              barColor = ['primary.main', 'secondary.main'],
                              value,
                              label = [null, null],
                              height = 20,
                              showBorder = true,
                          }) {
    const theme = useTheme();

    const barStyle = {
        height,
        pl: 0.5,
        pr: 0.5,
        overflow: 'hidden',
    }

    const textStyle = {
        height,
        lineHeight: `${height}px`,
        fontSize: getFontSize(height),
    }

    const val = value.map(v => (v * 100).toFixed(1));
    return <Box position={'relative'}
                {...barStyle}
                display={'flex'}
                bgcolor={trackColor}
                borderLeft={`${showBorder ? 1 : 0}px solid ${barColor[0]}`}
                borderRight={`${showBorder ? 1 : 0}px solid ${barColor[1]}`}>
        <Typography flex={'0 0 50%'}
                    {...textStyle}
                    pr={0.5}
                    textAlign={'right'}
                    color={theme.palette.getContrastText(trackColor)}>
            {label[0] || val[0]}
        </Typography>
        <Typography flex={'0 0 50%'}
                    {...textStyle}
                    pl={0.5}
                    textAlign={'left'}
                    color={theme.palette.getContrastText(trackColor)}>
            {label[1] || val[1]}
        </Typography>
        <Box position={'absolute'}
             {...barStyle}
             right={'50%'}
             width={`${val[0] / 2}%`}
             bgcolor={barColor[0]}>
            <Typography position={'absolute'}
                        right={0} pr={0.5}
                        width={'fit-content'}
                        color={theme.palette.getContrastText(barColor[0])}
                        {...textStyle}>
                {label[0] || val[0]}
            </Typography>
        </Box>
        <Box position={'absolute'}
             {...barStyle}
             left={'50%'}
             width={`${val[1] / 2}%`}
             bgcolor={barColor[1]}>
            <Typography position={'absolute'}
                        left={0} pl={0.5}
                        width={'fit-content'}
                        color={theme.palette.getContrastText(barColor[1])}
                        {...textStyle}>
                {label[1] || val[1]}
            </Typography>
        </Box>
    </Box>
}

function SingleTopBar({
                          trackColor = 'background.paper',
                          barColor = 'primary.main',
                          value,
                          label = null,
                          height = 20,
                          showBorder = true,
                      }) {
    const val = (value * 100).toFixed(1);
    return <Box position={'relative'}
                height={height}
                bgcolor={trackColor}
                borderRight={`${showBorder ? 1 : 0}px solid ${barColor}`}>
        <Typography position={'absolute'}
                    bottom={height + 2}
                    fontSize={getFontSize(height)}
                    height={height} lineHeight={`${height}px`}>
            {label || val}
        </Typography>
        <Box bgcolor={barColor}
             height={height}
             width={`${val}%`}/>
    </Box>
}

function SingleLeftBar({
                           trackColor = 'background.paper',
                           barColor = 'primary.main',
                           value,
                           label = null,
                           height = 20,
                           showBorder = true,
                           textWidth = 36,
                       }) {
    const val = (value * 100).toFixed(1);
    return <Box width={'100%'}
                display={'flex'}
                alignItems={'center'}>
        <Typography width={textWidth} flex={`0 0 ${textWidth}px`}
                    textAlign={"right"} pr={0.5}
                    fontSize={getFontSize(height)}
                    height={height} lineHeight={`${height}px`}>
            {label || val}
        </Typography>
        <Box flex={'1 1 0%'}
             overflow={'hidden'}
             height={height}
             bgcolor={trackColor}
             borderRight={`${showBorder ? 1 : 0}px solid ${barColor}`}>
            <Box bgcolor={barColor}
                 height={height}
                 width={`${val}%`}/>
        </Box>
    </Box>
}

function SingleOverlapBar({
                              trackColor = 'background.paper',
                              barColor = 'primary.main',
                              value,
                              label = null,
                              height = 20,
                              showBorder = true,
                          }) {
    const theme = useTheme();

    const commonStyle = {
        height,
        lineHeight: `${height}px`,
        fontSize: getFontSize(height),
        overflow: 'hidden',
        pl: 0.5,
        sx: {
            verticalAlign: 'middle',
        }
    }

    const val = (value * 100).toFixed(1);
    return <Box {...commonStyle}
                position={'relative'}
                borderRight={`${showBorder ? 1 : 0}px solid ${barColor}`}
                bgcolor={trackColor}
                color={theme.palette.getContrastText(trackColor)}>
        {label || val}
        <Box {...commonStyle}
             position={'absolute'}
             width={`${val}%`}
             top={0}
             left={0}
             bgcolor={barColor}
             color={theme.palette.getContrastText(barColor)}>
            {label || val}
        </Box>
    </Box>
}


const bars = {
    'SingleOverlap': SingleOverlapBar,
    'SingleTop': SingleTopBar,
    'SingleLeft': SingleLeftBar,
    'DoubleOverlap': DoubleOverlapBar,
    'DoubleTop': DoubleTopBar,
    'DoubleLeft': DoubleLeftBar,
}
export default memo(function Bar({
                                     variant = 'SingleOverlap',
                                     ...props
                                 }) {
    props.trackColor = props.trackColor || 'background.paper';
    props.barColor = props.barColor || (variant.startsWith('Single') ? 'primary.main' : ['primary.main', 'secondary.main']);

    const theme = useTheme();
    if (!isNormalColor(props.trackColor))
        props.trackColor = getJsonValFromDotChain(theme.palette, props.trackColor);
    if (variant.startsWith('Single')) {
        if (!isNormalColor(props.barColor))
            props.barColor = getJsonValFromDotChain(theme.palette, props.barColor);
    } else {
        for (const i of [0, 1])
            if (!isNormalColor(props.barColor[i]))
                props.barColor[i] = getJsonValFromDotChain(theme.palette, props.barColor[i]);
    }
    const BarChoice = bars[variant];
    return <BarChoice {...props}/>
})
