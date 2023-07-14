<script setup lang="ts">
import { computed } from 'vue';

interface Props {
  timestamp: number,
}

const props = defineProps<Props>();

const datetime = computed(() => {
  const date = new Date(props.timestamp * 1000);
  const options: Intl.DateTimeFormatOptions = {
    dateStyle: 'medium',
    timeStyle: 'short',
  };

  return Intl.DateTimeFormat(navigator.language, options).format(date);
});
</script>

<template>
<span>{{ datetime }}</span>
</template>
