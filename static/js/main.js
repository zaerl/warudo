const ctx = document.getElementById('main');

async function get_data() {
  const params = new URLSearchParams({
    limit: 2000
  });
  const response = await fetch('/app?' + params.toString());
  const results = await response.json();

  let methods = [];

  for(let result of results) {
    if(typeof result.method === 'undefined') {
      result.method = 'Unknown';
    }

    if(typeof methods[result.method] === 'undefined') {
      methods[result.method] = 0;
    }

    ++methods[result.method];
  }

  new Chart(ctx, {
    type: 'bar',
    data: {
      labels: Object.keys(methods),
      datasets: [{
        label: 'Calls',
        data: Object.values(methods),
        borderWidth: 1
        }]
      }, options: {
        scales: {
          y: {
            beginAtZero: true
          }
        }
      }
    }
  );
}

async function get_keys() {
  const response = await fetch('/app/keys');
  const results = await response.json();
  const select = document.getElementById('main-keys');

  for(const result of results) {
    const el = document.createElement('option');
    el.textContent = result.name;
    el.value = result.name;
    select.appendChild(el);
  }
}

async function init() {
  await Promise.all([get_data(), get_keys()]);
}

init();
