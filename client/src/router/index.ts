import { createRouter, createWebHistory } from 'vue-router'
// import EntriesView from '../views/EntriesView.vue'

const router = createRouter({
  history: createWebHistory(import.meta.env.BASE_URL),
  routes: [
    {
      path: '/',
      name: 'entries',
      component: () => import('../views/EntriesView.vue'),
      meta: {
        title: 'Entries',
      },
    },
    {
      path: '/entries/:id',
      name: 'entry',
      component: () => import('../views/EntryView.vue'),
      meta: {
        title: 'Entry',
      },
    },
    {
      path: '/views',
      name: 'views',
      component: () => import('../views/ViewsView.vue'),
      meta: {
        title: 'Views',
      },
    },
    {
      path: '/new-view',
      name: 'new-view',
      component: () => import('../views/NewViewView.vue'),
      meta: {
        title: 'New View',
      },
    },
    {
      path: '/views/:id',
      name: 'view',
      component: () => import('../views/ViewView.vue'),
      meta: {
        title: 'View',
      },
    },
    {
      path: '/settings',
      name: 'settings',
      component: () => import('../views/SettingsView.vue'),
      meta: {
        title: 'Settings',
      },
    },
    {
      path: '/:notFound',
      component: () => import('../views/NotFoundView.vue'),
    }
  ]
});

router.beforeEach((to, _from, next) => {
  document.title = to.meta.title as string;
  next();
});

export default router;
