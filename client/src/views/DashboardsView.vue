<script setup lang="ts">
import DateTime from '@/components/DateTime.vue';
import TableHeaderState, { type TableHeader } from '@/components/TableHeaderState.vue';
import TableRowState from '@/components/TableRowState.vue';
import { getData, type Dashboard, type OrderBy, type SearchBarParams, type Sort } from '@/data/api';
import { defaultHeaders } from '@/data/table';
import router from '@/router';
import { ref, watch } from 'vue';
import { RouterLink, useRoute } from 'vue-router';

const route = useRoute();

let dashboards = ref<Dashboard[] | null>([]);
let search = ref('');
let busy = ref(true);
let invalid = ref(false);
let headers = ref<TableHeader[]>(defaultHeaders('name', 'Name'));

async function fetchData() {
  busy.value = true;
  invalid.value = false;

  const query = search.value !== '' ? { key: 'any', value: search.value } : undefined;

  dashboards.value = await getData<Dashboard[]>('dashboards', query );

  busy.value = false;
  invalid.value = dashboards.value === null;
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
<main class="container">
  <table>
    <TableHeaderState :headers="headers" :sort="(route.query.sort as Sort)" :order-by="(route.query.orderby as OrderBy)" @sort="onSort" />
    <tbody>
      <TableRowState :busy="busy" :invalid="invalid" :count="dashboards?.length ?? 0" :columns="3" />
      <tr v-for="dashboard in dashboards" :key="dashboard.id" @click="router.push({ name: 'dashboard', params: { id: dashboard.id } })">
        <td>{{ dashboard.id }}</td>
        <td class="date"><DateTime :timestamp="dashboard.created" /></td>
        <td>{{ dashboard?.data?.name }}</td>
      </tr>
    </tbody>
  </table>
  <div>
    <RouterLink to="/new-dashboard" role="button" class="outline">Create dashboard</RouterLink>
  </div>
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
