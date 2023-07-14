<script setup lang="ts">
// import { getSettings } from '@/data/settings';

interface Props {
  timestamp: number,
}

const props = defineProps<Props>();
// const settings = getSettings();

function datetime() {
  const date = new Date(props.timestamp * 1000);
  const options: Intl.DateTimeFormatOptions = {
    dateStyle: 'medium',
    timeStyle: 'short',
  };

  return Intl.DateTimeFormat(navigator.language, options).format(date);
}
</script>

<template>
<span>{{ datetime() }}</span>
</template>
