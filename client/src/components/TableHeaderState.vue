<script setup lang="ts">
import type { OrderBy, Sort } from '@/data/api';

export interface TableHeader {
  key: string,
  name: string,
}

interface Props {
  headers: TableHeader[],
  orderBy: OrderBy,
  sort: Sort,
}

const props = defineProps<Props>();
defineEmits(['sort']);

function headerName(header: TableHeader) {
  if(header.key !== props.orderBy) {
    return header.name;
  }

  if(props.sort === 'asc') {
    return `${header.name} ▲`;
  }

  return `${header.name} ▼`;
}
</script>

<template>
<thead>
  <tr>
    <th v-for="(header) in props.headers" :key="header.key" @click="$emit('sort', header)">{{ headerName(header) }}</th>
  </tr>
</thead>
</template>

<style scoped>
th {
  cursor: pointer;
}
</style>
