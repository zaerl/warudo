<script setup lang="ts">
import { ref } from 'vue';

const isOpen = ref(false);
const emit = defineEmits(['cancel', 'confirm']);

function closeDialog() {
  isOpen.value = false;
  emit('cancel');
}

function confirm() {
  isOpen.value = false;
  emit('confirm');
}
</script>

<template>
<dialog :open="isOpen">
  <article>
    <header>
        <a aria-label="Close" class="close" @click="closeDialog"></a>
        Confirm?
    </header>
    <p>
      Cras sit amet maximus risus.
      Pellentesque sodales odio sit amet augue finibus pellentesque.
      Nullam finibus risus non semper euismod.
    </p>
    <footer>
      <a
        href=""
        role="button"
        class="secondary"
        @click.prevent="closeDialog">
        Cancel
      </a>
      <a
        href=""
        role="button"
        @click.prevent="confirm">
        Confirm
      </a>
    </footer>
  </article>
</dialog>
</template>

<style scoped></style>
