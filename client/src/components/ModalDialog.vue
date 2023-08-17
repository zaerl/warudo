<script setup lang="ts">

const emit = defineEmits(['close']);

interface Props {
  isOpen?: boolean,
  isDisabled?: true | null,
  title?: string,
  description?: string,
  cancelButton?: string,
  confirmButton?: string,
}

const props = withDefaults(
  defineProps<Props>(), {
    isDisabled: true,
    isOpen: false,
    title: '',
    description: '',
    cancelButton: 'Cancel',
    confirmButton: 'Confirm',
  }
);

function close() {
  emit('close', false);
}

function confirm() {
  emit('close', true);
}

</script>

<template>
<dialog :open="props.isOpen">
  <article>
    <a aria-label="Close" class="close" @click="close"></a>
    <h3>{{ props.title }}</h3>
    <slot>{{ props.description }}</slot>
    <footer>
      <a
        href=""
        role="button"
        class="secondary"
        @click.prevent="close">
        {{ props.cancelButton }}
      </a>
      <a
        href="#"
        role="button"
        :disabled="props.isDisabled"
        @click.prevent="confirm">
        {{ props.confirmButton }}
      </a>
    </footer>
  </article>
</dialog>
</template>

<style scoped>
.close {
  cursor: pointer;
}
</style>
