import React, { useMemo } from 'react';
import {
  Col,
  Center,
  Rule,
  Row,
  Box,
  Text,
  Icon
} from '@tlon/indigo-react';
import {
  Track,
  useCurrentTrack,
  useFormattedSeek,
  useFutureQueue,
  useTransport
} from '~/logic/state/audio';
import { Dropdown } from './Dropdown';
import { TruncatedText } from './TruncatedText';
import { MarqueeText } from './MarqueeText';
import { PropFunc } from '~/types';
import { useAssocForGraph } from '~/logic/state/metadata';

export const Backdrop = (
  props: React.PropsWithChildren<PropFunc<typeof Box>>
) => (
  <Box
    {...props}
    border="1"
    borderColor="lightGray"
    borderRadius="2"
    backgroundColor="white"
  />
);

function QueuedTrack(props: { track: Track; first?: boolean }) {
  const { track, first = false } = props;
  return (
    <>
      {first ? null : <Rule />}
      <Row mb={2} mt={first ? 0 : 2}>
        <TruncatedText>{track.title}</TruncatedText>
      </Row>
    </>
  );
}

function StatusBarMediaDropdown() {
  const [queue, resource] = useFutureQueue();
  const association = useAssocForGraph(resource);
  const title = association?.metadata?.title ?? resource;

  return (
    <Col gapY="2">
      <Row p="1">
        <Text fontWeight="medium">Next up from {title}</Text>
      </Row>
      <Col p="1">
        {queue.map((t, i) => (
          <QueuedTrack first={i === 0} key={i} track={t} />
        ))}
        {queue.length === 0 ? (
          <Row>
            <Text gray>Nothing queued</Text>
          </Row>
        ) : null}
      </Col>
    </Col>
  );
}

type StatusBarMediaTransportProps = ReturnType<typeof useTransport> &
  ReturnType<typeof useFormattedSeek> &
  PropFunc<typeof Row> & {
    title: string;
  };

function StatusBarMediaTransport(
  props: React.PropsWithChildren<StatusBarMediaTransportProps>
) {
  const {
    title,
    progress,
    duration,
    playing,
    pause,
    play,
    children,
    next,
    prev,
    ...rest
  } = props;

  return (
    <Row
      {...rest}
      flexDirection={['column', 'row']}
      borderRadius="2"
      backgroundColor="white"
      alignItems="center"
      justifyContent="space-between"
      borderColor="lightGray"
    >
      <Row
        flexDirection={['column', 'row']}
        pl="2"
        gapX="2"
        alignItems="center"
      >
        <Row mb={[2, 0]} height="32px" alignItems="center" gapX="2">
          <Icon
            size="24px"
            onClick={prev}
            display="inline-block"
            icon="Previous"
          />
          <Icon
            size="24px"
            display="inline-block"
            onClick={playing ? pause : play}
            icon={playing ? 'Pause' : 'Play'}
          />
          <Icon
            size="24px"
            onClick={next}
            display="inline-block"
            icon="Next"
          />
        </Row>
        <Row justifyContent="center" width="280px" height="32px" flexGrow={1}>
          <MarqueeText>{title ?? 'Unknown track'}</MarqueeText>
        </Row>
      </Row>
      {/* have to have constant width to prevent reflow jank */}
      <Row
        mb={[2, 0]}
        height="32px"
        alignItems="center"
        justifyContent="flex-end"
      >
        <>
          <Row pr="2" justifyContent="flex-end" width="72px">
            <Text gray>{progress}</Text><Text>/{duration}</Text>
          </Row>
          {children}
        </>
      </Row>
    </Row>
  );
}

export function StatusBarMedia(props: {}) {
  const current = useCurrentTrack();
  const transport = useTransport();
  const seek = useFormattedSeek();

  const options = useMemo(
    () => (
      <Backdrop maxWidth="300px" p="2">
        <StatusBarMediaDropdown />
      </Backdrop>
    ),
    []
  );

  if (!current) {
    return <Box />;
  }

  return (
    <>
      <Dropdown
        display={['block', 'none']}
        alignY="top"
        alignX="right"
        offsetX={70}
        options={
          <Backdrop p="2" width="300px">
            <StatusBarMediaTransport
              title={current?.title ?? 'Unknown Track'}
              {...seek}
              {...transport}
            />
            <StatusBarMediaDropdown />
          </Backdrop>
        }
      >
        <Row
          justifyContent="center"
          alignItems="center"
          backgroundColor="white"
          borderRadius="2"
          height="32px"
          border="1"
          borderColor="lightGray"
          width="32px"
        >
          <Icon display="inline-block" icon={transport.playing ? 'Pause' : 'Play'} />
        </Row>
      </Dropdown>
      <StatusBarMediaTransport
        display={['none', 'flex']}
        title={current?.title ?? 'Unknown Track'}
        border={1}
        {...seek}
        {...transport}
      >
        <Center width="32px" height="32px">
          <Dropdown alignY="top" alignX="right" options={options}>
            <Icon size="24px" display="inline-block" icon="Gear" />
          </Dropdown>
        </Center>
      </StatusBarMediaTransport>
    </>
  );
}

