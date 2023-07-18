<script setup lang="ts">
import DateTime from '@/components/DateTime.vue';
import JSONBlock from '@/components/JSONBlock.vue';
import SearchBar from '@/components/SearchBar.vue';
import TableHeaderState, { type TableHeader } from '@/components/TableHeaderState.vue';
import TableRowState from '@/components/TableRowState.vue';
import type { Entry, OrderBy, SearchBarParams, Sort } from '@/data/api';
import { getData } from '@/data/api';
import router from '@/router';
import { ref, watch } from 'vue';
import { useRoute } from 'vue-router';

const route = useRoute();

let entries = ref<Entry[] | null>([]);
let busy = ref(true);
let invalid = ref(false);
let search = ref('');
let orderBy = ref<OrderBy>(null);
let sort = ref<Sort>(null);
let headers = ref<TableHeader[]>([
  {
    key: 'id',
    name: '#',
  },
  {
    key: 'created',
    name: 'Created',
  },
  {
    key: 'data',
    name: 'Data',
  },
]);

async function fetchData() {
  busy.value = true;
  let query: SearchBarParams = {};

  if(search.value !== '') {
    query.key = 'any';
    query.value = search.value;
  }

  if(sort.value !== null) {
    query.sort = sort.value;
  }

  if(orderBy.value !== null) {
    query.orderby = orderBy.value;
  }

  entries.value = await getData<Entry[]>('entries', query );
  busy.value = false;

  router.replace({ ...route, query: query as any });
}

watch(orderBy, fetchData);
watch(sort, fetchData);
watch(search, fetchData);

fetchData();

function onSort(header: TableHeader) {
  orderBy.value = header.key as OrderBy;

  if(sort.value === null) {
    sort.value = 'asc';
  } else {
    sort.value = sort.value === 'asc' ? 'desc' : 'asc';
  }

  orderBy.value = header.key as OrderBy;
}
</script>

<template>
<SearchBar v-model="search" />
<main class="container-fluid">
  <table>
    <TableHeaderState :headers="headers" :sort="sort" :order-by="orderBy" @sort="onSort" />
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
