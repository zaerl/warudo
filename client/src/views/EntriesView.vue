<script setup lang="ts">
import DateTime from '@/components/DateTime.vue';
import JSONBlock from '@/components/JSONBlock.vue';
import SearchBar from '@/components/SearchBar.vue';
import TableHeaderState, { type TableHeader } from '@/components/TableHeaderState.vue';
import TableRowState from '@/components/TableRowState.vue';
import type { Entry, OrderBy, SearchBarParams, Sort } from '@/data/api';
import { getData } from '@/data/api';
import { defaultHeaders, getQuery } from '@/data/table';
import router from '@/router';
import { ref, watch } from 'vue';
import { useRoute } from 'vue-router';

const route = useRoute();

let entries = ref<Entry[] | null>([]);
let search = ref('');
let busy = ref(true);
let invalid = ref(false);
let headers = ref<TableHeader[]>(defaultHeaders());

async function fetchData() {
  busy.value = true;
  invalid.value = false;

  const query = getQuery(search.value, route.query);

  entries.value = await getData<Entry[]>('entries', query );
  busy.value = false;
  invalid.value = entries.value === null;
}

watch(route, fetchData, { immediate: true });
watch(search, fetchData);

function onSort(header: TableHeader) {
  let query: SearchBarParams = {};
  query.orderby = header.key as OrderBy;

  if(typeof route.query.sort === 'undefined') {
    query.sort = 'asc';
  } else {
    query.sort = route.query.sort === 'asc' ? 'desc' : 'asc';
  }

  router.replace({ ...route, query: query as any  });
}
</script>

<template>
<SearchBar v-model="search" />
<main class="container-fluid">
  <table>
    <TableHeaderState :headers="headers" :sort="(route.query.sort as Sort)" :order-by="(route.query.orderby as OrderBy)" @sort="onSort" />
    <tbody>
      <TableRowState :busy="busy" :invalid="invalid" :count="entries?.length ?? 0" :columns="3" />
      <tr v-for="entry in entries" :key="entry.id" @click="router.push({ name: 'entry', params: { id: entry.id } })">
        <td>{{ entry.id }}</td>
        <td class="date"><DateTime :timestamp="entry.created" /></td>
        <td>
          <!--<samp><span v-for="(value, key, index) in entry.data"><span v-if="index !== 0">, </span><strong>{{ key }}</strong>: {{ value }}</span></samp>-->
          <JSONBlock :json="entry.data" :inline="true" />
        </td>
      </tr>
    </tbody>
  </table>
</main>
</template>

<style scoped>
tr {
  cursor: pointer;
}

td.date {
  white-space: pre;
}
</style>
