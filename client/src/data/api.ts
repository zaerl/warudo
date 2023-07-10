export interface SearchParams {
    type: 'entries' | 'views';
    limit?: number;
    key?: string;
    value?: string;
}

export async function getData(search: SearchParams = { type: 'entries' }) {
    const params = new URLSearchParams(search as unknown as Record<string, string>);

    const response = await fetch(`/api/data?${params}`);
    const data = await response.json();
    return data;
}
