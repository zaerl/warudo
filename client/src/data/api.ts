export type GetDataType = 'keys' | 'entries' | 'views';

export interface SearchParams {
  limit?: number;
  key?: string;
  value?: string;
}

export async function getData<T>(type: GetDataType = 'entries', search: SearchParams = {}): Promise<T | null> {
    const params = new URLSearchParams(search as unknown as Record<string, string>);

    try {
        const response = await fetch(`http://localhost:6252/app/${type}?${params}`);
        const data = await response.json();
        console.log(data);

        return data;
    } catch(error: unknown) {
        if (error instanceof Error) {
            console.log(error.message);
        } else {
            console.log(error);
        }

        return null;
    }
}
