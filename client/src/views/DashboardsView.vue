<script setup lang="ts">
import DateTime from '@/components/DateTime.vue';
import TableRowState from '@/components/TableRowState.vue';
import { getData, type Dashboard } from '@/data/api';
import router from '@/router';
import { ref } from 'vue';
import { RouterLink } from 'vue-router';

let dashboards = ref<Dashboard[] | null>([]);
let busy = ref(true);
let invalid = ref(false);
let search = ref('');

async function fetchData() {
  busy.value = true;
  const query = search.value !== '' ? { key: 'any', value: search.value } : undefined;
  dashboards.value = await getData<Dashboard[]>('dashboards', query );
  console.log(dashboards.value);
  busy.value = false;
  invalid.value = dashboards.value === null;
}

fetchData();
</script>

<template>
<main class="container">
  <table>
    <thead>
      <tr>
        <td>#</td>
        <td>Created</td>
        <td>Name</td>
      </tr>
    </thead>
    <tbody>
      <TableRowState :busy="busy" :invalid="invalid" :count="dashboards?.length ?? 0" :columns="3" />
      <tr v-for="dashboard in dashboards" :key="dashboard.id" @click="router.push({ name: 'dashboard', params: { id: dashboard.id } })">
        <td>{{ dashboard.id }}</td>
        <td><DateTime :timestamp="dashboard.created" /></td>
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
</style>
