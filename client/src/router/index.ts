import { createRouter, createWebHistory } from 'vue-router';

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
      path: '/dashboards',
      name: 'dashboards',
      component: () => import('../views/DashboardsView.vue'),
      meta: {
        title: 'Dashboards',
      },
    },
    {
      path: '/new-dashboard',
      name: 'new-dashboard',
      component: () => import('../views/NewDashboardView.vue'),
      meta: {
        title: 'New Dashboard',
      },
    },
    {
      path: '/dashboards/:id',
      name: 'dashboard',
      component: () => import('../views/DashboardView.vue'),
      meta: {
        title: 'Dashboards',
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
      meta: {
        title: '404 :(',
      },
    }
  ]
});

router.beforeEach((to, _from, next) => {
  document.title = to.meta.title as string;
  next();
});

export default router;
