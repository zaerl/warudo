import { createRouter, createWebHistory } from 'vue-router'
// import EntriesView from '../views/EntriesView.vue'

const router = createRouter({
  history: createWebHistory(import.meta.env.BASE_URL),
  routes: [
    {
      path: '/',
      name: 'entries',
      // component: EntriesView
      component: () => import('../views/EntriesView.vue')
    },
    {
      path: '/entries/:id',
      name: 'entry',
      component: () => import('../views/EntryView.vue')
    },
    {
      path: '/views',
      name: 'views',
      component: () => import('../views/ViewsView.vue')
    },
    {
      path: '/views/:id',
      name: 'view',
      component: () => import('../views/ViewView.vue')
    },
    {
      path: '/settings',
      name: 'settings',
      component: () => import('../views/SettingsView.vue')
    }
  ]
})

export default router
