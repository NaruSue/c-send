from __future__ import annotations

import argparse
import json
from datetime import datetime, timezone
from pathlib import Path
from urllib.parse import quote


def read_json(path: Path):
    with path.open(encoding="utf-8") as stream:
        return json.load(stream)


def dataset_entry(path: Path, payload: dict, language: str) -> dict:
    items = payload.get("items", [])
    api_ids = sorted({item.get("options", {}).get("api") for item in items if item.get("options", {}).get("api")})
    has_clipboard = any("{{clipboard}}" in str(item.get("value", "")) for item in items)
    has_api = any(item.get("mode") == "api" for item in items)
    return {
        "id": f"{language}-{path.stem}",
        "name": payload.get("categoryName") or path.stem,
        "language": language,
        "datasetType": "ai-prompt" if has_api else "dataset",
        "inputMode": "clipboard" if has_clipboard else "fixed",
        "itemCount": len(items),
        "requiredApis": api_ids,
        "downloadUrl": f"./datasets/{language}/{quote(path.name)}",
    }


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--root", type=Path, default=Path("."))
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()
    root = args.root.resolve()
    output = args.output.resolve()
    apis = []
    for path in sorted((root / "api").glob("*.json")):
        payload = read_json(path)
        apis.append({
            "id": payload["id"],
            "name": payload.get("name", payload["id"]),
            "authType": payload.get("authType"),
            "actionCount": len(payload.get("actions", [])),
            "downloadUrl": f"./api/{quote(path.name)}",
        })
    datasets = []
    for language, directory in (("ja", root / "samples" / "jp"), ("en", root / "samples" / "en")):
        for path in sorted(directory.glob("*.json")):
            datasets.append(dataset_entry(path, read_json(path), language))
    output.mkdir(parents=True, exist_ok=True)
    catalog = {
        "format": "c-send-library-catalog",
        "version": 1,
        "generatedAt": datetime.now(timezone.utc).isoformat(),
        "apis": apis,
        "datasets": datasets,
    }
    (output / "catalog.json").write_text(json.dumps(catalog, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
