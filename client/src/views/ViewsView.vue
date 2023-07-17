<script setup lang="ts">
import DateTime from '@/components/DateTime.vue';
import TableRowState from '@/components/TableRowState.vue';
import { getData, type View } from '@/data/api';
import router from '@/router';
import { ref } from 'vue';
import { RouterLink } from 'vue-router';

let views = ref<View[] | null>([]);
let busy = ref(true);
let invalid = ref(false);
let search = ref('');

async function fetchData() {
  busy.value = true;
  const query = search.value !== '' ? { key: 'any', value: search.value } : undefined;
  views.value = await getData<View[]>('views', query );
  busy.value = false;
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
      <TableRowState :busy="busy" :invalid="invalid" :count="views?.length ?? 0" :columns="3" />
      <tr v-for="view in views" :key="view.id" @click="router.push({ name: 'view', params: { id: view.id } })">
        <td>{{ view.id }}</td>
        <td><DateTime :timestamp="view.created" /></td>
        <td>{{ view?.data?.name }}</td>
      </tr>
    </tbody>
  </table>
  <div>
    <RouterLink to="/new-view" role="button" class="outline">Create a new view</RouterLink>
  </div>
</main>
</template>

<style scoped>
tr {
  cursor: pointer;
}
</style>
