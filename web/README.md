# c-send Template Sharing

The c-send portal is at `https://loose.bz/c-send/` and includes template management under `https://loose.bz/c-send/t/`. The official c-send PWA is published at `https://narusue.github.io/c-send.pwa/`; the legacy `https://loose.bz/c-send/pwa/` route remains available for existing users and automatically moves to the Pages version when an update is detected. It includes a read-only synced reference to the portal sample catalog. The PWA, portal, and template-management section share one account and profile. Google sign-in for the PWA uses the portal login flow and exchanges a short-lived authorization code for c-send API tokens; see [the API specification](api/README.md). Templates stay on the device by default. While signed in, users may enable automatic synchronization in their profile and opt in per template during create or edit; only templates with both permissions are synchronized as private drafts through the authenticated API. The sync options are hidden while signed out. Use the portal's template-management section to publish.

See [OPERATIONS.md](OPERATIONS.md) for the current deployment, decisions, initial accounts, maintenance notes, and known limitations.

PWA sign-in opens the portal's shared account login and registration screens. The PWA, portal, template-management section, and profile use the same c-send account. When a signed-in PWA user opens the portal homepage, template management, or profile settings, a short-lived single-use handoff establishes the portal session first. Account registration and profile editing are part of the portal; PWA account actions and profile-level plus per-item automatic synchronization use the shared API. Template content, version, and update time are synchronized; list order and other device-local usage state remain local to each device.

When deleting a synchronized template, the PWA distinguishes between deleting it from the current device, excluding it from synchronization on the current device, and deleting it from the server. The server-delete option requires authentication and propagates the deletion to other devices at their next synchronization. If the device is offline, the server deletion remains queued until reconnection. Deleted template identifiers are retained server-side for a limited period so an old backup cannot silently recreate the same server item; a deliberate re-registration is treated as a new template.

## Main paths

- `https://narusue.github.io/c-send.pwa/`: official c-send.web PWA. It stores items on device, supports JSON backup/restore, and backs up user templates to the `.tmp` database while signed in.
- `/c-send/`: c-send portal with two clear entry points (c-send.web app and c-send.tmp template-sharing site), a short usage overview, contact link, and legal/help pages.
- `/c-send/t/guide`, `/c-send/t/terms`, and `/c-send/t/privacy`: shared-layout guidance and policy pages. Signed-in visitors get the standard navigation; signed-out visitors retain the shared header without a menu. Legacy `.html` URLs redirect to these pages.
- `/c-send/t/`: public landing page. Signed-out visitors see sample and public-template entry cards plus login and registration actions; the management menu is hidden.
- `/c-send/t/login`, `/c-send/t/register`, `/c-send/t/reset-password`: Google or email-code sign-in, email verification registration, and email-code password reset.
- `/c-send/t/browse.php`: selectable public template list and share URL generator; template titles open the detail page.
- `/c-send/t/view.php?id={uuid}`: one template item's content, tags, author profile link, update time, and revision. Signed-in users can access permitted JSON/edit/report actions; signed-out visitors only browse public content.
- `/c-send/t/profile?id={uuid}`: public profile view; the owner can edit nickname, profile text, and a square 200×200 uploaded avatar.
- `/c-send/t/dashboard.php`: responsive template manager. Each site template is one independently managed c-send item. Selecting a row opens its editor, while `新規追加` opens a blank editor. Navigation is a desktop sidebar and a floating mobile menu. A saved c-send2 Export JSON can create a new template from the blank editor or update the selected template.
- `/c-send/t/admin-users.php`: user status management.
- `/c-send/t/admin-templates.php`: template review, history, moderation, sample designation, keyword/tag/user/status/report filters, and sort controls.
- `/c-send/t/admin-reports.php`: open reports; 25 reports per page.
- `/c-send/t/admin.php`: compatibility entry point that redirects to user management.
- `/c-send/t/{uuid}.json`: a single published template.
- `/c-send/t/selection.json?ids={uuid},{uuid}`: a combined document generated from the ordered GET parameter on every request.
- `/c-send/t/engagement`: portal-only template selection and like events, used for public counts and signed-in creator summaries. It does not connect to c-send.web.

The template portal puts the user guide, terms, and privacy links in the signed-in navigation and on the signed-out landing page; the registration screen also links them beside the account actions. The c-send.web app menu links to the same user guide.

The text-only portal files are included in every deployment package and published under `/home/web/public/c-send/`. To include the c-send.web PWA files too, run `./prepare-deploy.ps1 -IncludePwa`; those files are published under `/home/web/public/c-send/pwa/`. For auth-only updates, `deploy-google-auth.ps1` applies the Google-account identity migration and updates the `.tmp` login and registration pages without changing database credentials or PWA files.

All HTML pages share the same global header and responsive navigation pattern: a left sidebar on desktop and a floating menu on mobile. Public pages link to home and public templates; signed-in pages also link to the user's templates, admin menu when applicable, and logout. The main global header remains consistent across all pages.

The owner exports a one-item JSON file from c-send2, then selects that saved file in the template manager. From `新規追加` it creates a template; from an existing template's editor it updates that template. This is a normal browser file upload; c-send2 does not connect to the website directly. The editor determines the target template, so the exported JSON does not need a website ID. Uploading unchanged content leaves the revision and update date unchanged. Changed content creates a revision, updates the date, and returns the template to administrator review. Multiple items are combined into a JSON URL only when a visitor selects multiple independent templates in the public selector.

The share URL appears beside a copy button after selection. Copy feedback is announced in the page, with a mobile Safari fallback.

The selection URL is not stored. JSON bodies are stored as PostgreSQL `jsonb`; account, template identity, revision, and moderation metadata are separate. IDs remain stable across revisions and are metadata only, not fields in client JSON.

## Deploy packaging

`prepare-deploy.ps1` prepares an SSH transfer package and generates the two initial passwords. It does not start a local server. It uses Windows `tar.exe` to create a Linux-compatible tar.gz; do not replace this with `Compress-Archive`, which creates ZIP entries with Windows path separators. The server installer skips initial accounts that already exist.

Never commit `.env`, deployment credentials, generated user passwords, or the private server config. Server configuration is stored outside the document root at `/etc/csend-template/config.php`.

## Common JSON 1.1.0 specification (implementation pending)

The canonical [data-file specification](../docs/data-file-spec.md) defines integer `version`, `versionMinor`, and `versionPatch` fields; optional document-level `option` and item-level `items[].option`; author metadata; history with optional `previousValue`, `diff`, and `diffFormat`; and app-specific metadata under `apps`. Singular `option` is distinct from item `options`, which configures behavior.

Clients choose which optional metadata to use but must preserve unused and unknown data when saving. History-free export removes only document and item `option.history` from an independent export copy, preserving the source and all other metadata. The portal can store history in the template JSON without a dedicated history table. This does not replace existing revision, conflict, or audit records. The specification update does not implement metadata preservation, history editing, or history-free export in the PWA, portal, or API.

## PWA maintenance snapshot (2026-11-05)

The current production PWA is **0.1.85**. The source and GitHub Pages artifact live in `web/pwa/`; `scripts-ver/pwa/` is legacy/reference and must not be used as a deployment source. The `loose.bz/c-send/pwa/` URL is a compatibility deployment of the same current PWA.
Connection-list account and sync-settings screens return to the connection list when closed.
Developer-only implementation notes are in [the hidden-feature specification](../docs/pwa-easter-egg.md) and [release history](../docs/pwa-connections-release.md). Keep the hidden feature out of public news and normal application navigation.
Diagnostic submission continues to use the official-account admin panel and existing API, not a separate game service.
Clearing old diagnostics on version upgrades is **pending for the next update**, as specified in [pwa-next-update.md](../docs/pwa-next-update.md); it is not shipped in 0.1.85.
