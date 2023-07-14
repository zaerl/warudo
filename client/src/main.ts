import './assets/main.css'

import { createApp } from 'vue'
import App from './App.vue'
import { initCharts } from './charts'
import router from './router'

initCharts();

const app = createApp(App)

app.use(router)
app.mount('#app')
