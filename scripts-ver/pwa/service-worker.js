const CACHE_NAME = "c-send-pwa-v2.8";
const ASSETS = [
  "./",
  "./index.html",
  "./manifest.webmanifest",
  "./service-worker.js",
  "./news.html",
  "./samples.json",
  "./apple-touch-icon.png",
  "./c-send.ico"
  ,"./api/gemini.json"
  ,"./samples/jp/AI API.json"
];

self.addEventListener("install", (event) => {
  event.waitUntil(
    caches.open(CACHE_NAME).then((cache) => cache.addAll(ASSETS))
  );
  self.skipWaiting();
});

self.addEventListener("activate", (event) => {
  event.waitUntil(
    caches.keys().then((keys) =>
      Promise.all(
        keys
          .filter((key) => key !== CACHE_NAME)
          .map((key) => caches.delete(key))
      )
    )
  );
  self.clients.claim();
});

self.addEventListener("fetch", (event) => {
  if (event.request.method !== "GET") return;

  const requestUrl = new URL(event.request.url);
  if (requestUrl.pathname.endsWith("/news.html") && requestUrl.searchParams.has("newsCheck")) {
    event.respondWith(fetch(event.request, { cache: "no-store" }));
    return;
  }
  if (requestUrl.pathname.endsWith("/news.html") && requestUrl.searchParams.has("show")) {
    event.respondWith(
      fetch(event.request, { cache: "no-store" }).catch(() => caches.match(new URL("./news.html", self.location.href).href))
    );
    return;
  }

  event.respondWith(
    caches.match(event.request).then((cached) => {
      if (cached) return cached;
      return fetch(event.request).then((response) => {
        const cloned = response.clone();
        caches.open(CACHE_NAME).then((cache) => {
          cache.put(event.request, cloned);
        });
        return response;
      }).catch(() => caches.match("./index.html"));
    })
  );
});
