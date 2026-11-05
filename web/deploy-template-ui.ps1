param(
    [string]$SshTarget = 'c-send-org-deploy',
    [string]$SshConfig = ''
)

$ErrorActionPreference = 'Stop'
$webRoot = Split-Path -Parent $MyInvocation.MyCommand.Path
$deployDir = Join-Path $env:TEMP ('csend-template-ui-' + [guid]::NewGuid().ToString('N'))
$stage = Join-Path $deployDir 'package'
New-Item -ItemType Directory -Path (Join-Path $stage 'public') -Force | Out-Null
New-Item -ItemType Directory -Path (Join-Path $stage 'app') -Force | Out-Null
$files = @('account-status.php','index.php','login.php','avatar.php','register.php','reset-password.php','profile.php','dashboard.php','browse.php','view.php','engagement.php','admin-templates.php','admin-users.php','template.php','upload.php','google-callback.php','guide.php','terms.php','privacy.php')
foreach ($file in $files) { Copy-Item (Join-Path $webRoot "public\$file") (Join-Path $stage "public\$file") }
New-Item -ItemType Directory -Path (Join-Path $stage 'portal') -Force | Out-Null
Copy-Item (Join-Path $webRoot 'portal\index.html') (Join-Path $stage 'portal\index.html')
Copy-Item (Join-Path $webRoot 'app\navigation.php') (Join-Path $stage 'app\navigation.php')
Copy-Item (Join-Path $webRoot 'app\bootstrap.php') (Join-Path $stage 'app\bootstrap.php')
Copy-Item (Join-Path $webRoot 'app\site-layout.php') (Join-Path $stage 'app\site-layout.php')
Copy-Item (Join-Path $webRoot 'app\admin-layout.php') (Join-Path $stage 'app\admin-layout.php')
New-Item -ItemType Directory -Path (Join-Path $stage 'database\migrations') -Force | Out-Null
New-Item -ItemType Directory -Path (Join-Path $stage 'api\v1') -Force | Out-Null
New-Item -ItemType Directory -Path (Join-Path $stage 'bin') -Force | Out-Null
Copy-Item (Join-Path $webRoot 'database\migrations\011_email_accounts.sql') (Join-Path $stage 'database\migrations\011_email_accounts.sql')
Copy-Item (Join-Path $webRoot 'database\migrations\012_split_template_items.sql') (Join-Path $stage 'database\migrations\012_split_template_items.sql')
Copy-Item (Join-Path $webRoot 'database\migrations\013_email_encryption_columns.sql') (Join-Path $stage 'database\migrations\013_email_encryption_columns.sql')
Copy-Item (Join-Path $webRoot 'database\migrations\014_email_encryption_indexes.sql') (Join-Path $stage 'database\migrations\014_email_encryption_indexes.sql')
Copy-Item (Join-Path $webRoot 'database\migrations\015_template_engagement.sql') (Join-Path $stage 'database\migrations\015_template_engagement.sql')
Copy-Item (Join-Path $webRoot 'bin\encrypt-email-data.php') (Join-Path $stage 'bin\encrypt-email-data.php')
Copy-Item (Join-Path $webRoot 'bin\verify-email-login.php') (Join-Path $stage 'bin\verify-email-login.php')
Copy-Item (Join-Path $webRoot 'api\v1\index.php') (Join-Path $stage 'api\v1\index.php')
Copy-Item (Join-Path $webRoot 'nginx-c-send-locations.conf') (Join-Path $stage 'nginx-c-send-locations.conf')
$archive = Join-Path $deployDir 'template-ui.tar.gz'
$remoteScript = Join-Path $deployDir 'deploy.sh'
$script = @'
#!/usr/bin/env bash
set -euo pipefail
ARCHIVE=/tmp/csend-template-ui.tar.gz
STAGE=$(mktemp -d /tmp/csend-template-ui.XXXXXX)
BACKUP=/home/web/.c-send-template-ui-backups/release-$(date -u +%Y%m%dT%H%M%SZ)
cleanup() { rm -rf "$STAGE" "$ARCHIVE"; }
trap cleanup EXIT
chmod 600 "$ARCHIVE"
tar -xzf "$ARCHIVE" -C "$STAGE"
for file in "$STAGE"/public/*.php; do php -l "$file"; done
for file in "$STAGE"/app/*.php; do php -l "$file"; done
php -l "$STAGE/bin/encrypt-email-data.php"
php -l "$STAGE/bin/verify-email-login.php"
php -l "$STAGE/api/v1/index.php"
sudo install -d -o root -g root -m 0755 "$(dirname "$BACKUP")"
sudo install -d -o root -g root -m 0755 "$BACKUP/public" "$BACKUP/app" "$BACKUP/api/v1" "$BACKUP/bin" "$BACKUP/database/migrations" "$BACKUP/portal"
if sudo test -e /home/web/public/c-send/index.html; then sudo cp -a /home/web/public/c-send/index.html "$BACKUP/portal/index.html"; fi
for file in "$STAGE"/public/*.php; do name=$(basename "$file"); source="/home/web/public/c-send/t/$name"; if sudo test -e "$source"; then sudo cp -a "$source" "$BACKUP/public/$name"; fi; done
if sudo test -e /home/web/public/c-send/api/v1/index.php; then sudo cp -a /home/web/public/c-send/api/v1/index.php "$BACKUP/api/v1/index.php"; fi
sudo cp -a /home/web/c-send-private/app/navigation.php "$BACKUP/app/navigation.php"
sudo cp -a /home/web/c-send-private/app/bootstrap.php "$BACKUP/app/bootstrap.php"
sudo cp -a /home/web/c-send-private/app/site-layout.php "$BACKUP/app/site-layout.php"
sudo cp -a /home/web/c-send-private/app/admin-layout.php "$BACKUP/app/admin-layout.php"
sudo cp -a /etc/nginx/snippets/csend-template.conf "$BACKUP/nginx-c-send-locations.conf"
sudo cp -a "$STAGE/database/migrations/012_split_template_items.sql" "$BACKUP/database/migrations/012_split_template_items.sql"
sudo cp -a "$STAGE/database/migrations/013_email_encryption_columns.sql" "$BACKUP/database/migrations/013_email_encryption_columns.sql"
sudo cp -a "$STAGE/database/migrations/014_email_encryption_indexes.sql" "$BACKUP/database/migrations/014_email_encryption_indexes.sql"
sudo cp -a "$STAGE/database/migrations/015_template_engagement.sql" "$BACKUP/database/migrations/015_template_engagement.sql"
sudo install -d -o root -g www-data -m 0750 /etc/csend-template
if ! sudo test -s /etc/csend-template/.env; then
  key=$(openssl rand -base64 32 | tr -d '\n')
  sudo install -o root -g www-data -m 0640 /dev/null /etc/csend-template/.env
  printf 'EMAIL_ENCRYPTION_KEY=%s\n' "$key" | sudo tee -a /etc/csend-template/.env >/dev/null
  unset key
elif ! sudo grep -q '^EMAIL_ENCRYPTION_KEY=' /etc/csend-template/.env; then
  key=$(openssl rand -base64 32 | tr -d '\n')
  printf 'EMAIL_ENCRYPTION_KEY=%s\n' "$key" | sudo tee -a /etc/csend-template/.env >/dev/null
  unset key
  sudo chown root:www-data /etc/csend-template/.env
  sudo chmod 0640 /etc/csend-template/.env
fi
sudo chown root:www-data /etc/csend-template/.env
sudo chmod 0640 /etc/csend-template/.env
sudo -u postgres psql -d csend_templates -v ON_ERROR_STOP=1 -f "$STAGE/database/migrations/011_email_accounts.sql"
sudo -u postgres psql -d csend_templates -v ON_ERROR_STOP=1 -f "$STAGE/database/migrations/012_split_template_items.sql"
sudo -u postgres psql -d csend_templates -v ON_ERROR_STOP=1 -f "$STAGE/database/migrations/013_email_encryption_columns.sql"
sudo -u postgres psql -d csend_templates -v ON_ERROR_STOP=1 -f "$STAGE/database/migrations/015_template_engagement.sql"
sudo -u postgres psql -d csend_templates -v ON_ERROR_STOP=1 -c "GRANT SELECT,INSERT,DELETE ON template_engagement_events TO csend_templates_app; GRANT USAGE,SELECT ON SEQUENCE template_engagement_events_id_seq TO csend_templates_app;"
for file in "$STAGE"/public/*.php; do sudo install -o www-data -g www-data -m 0644 "$file" "/home/web/public/c-send/t/$(basename "$file")"; done
sudo install -o www-data -g www-data -m 0644 "$STAGE/portal/index.html" /home/web/public/c-send/index.html
sudo install -o root -g www-data -m 0640 "$STAGE/app/navigation.php" /home/web/c-send-private/app/navigation.php
sudo install -o root -g www-data -m 0640 "$STAGE/app/bootstrap.php" /home/web/c-send-private/app/bootstrap.php
sudo install -o root -g www-data -m 0640 "$STAGE/app/site-layout.php" /home/web/c-send-private/app/site-layout.php
sudo install -o root -g www-data -m 0640 "$STAGE/app/admin-layout.php" /home/web/c-send-private/app/admin-layout.php
sudo install -o root -g www-data -m 0640 "$STAGE/bin/encrypt-email-data.php" /home/web/c-send-private/bin/encrypt-email-data.php
sudo install -o root -g www-data -m 0640 "$STAGE/bin/verify-email-login.php" /home/web/c-send-private/bin/verify-email-login.php
sudo install -o www-data -g www-data -m 0644 "$STAGE/api/v1/index.php" /home/web/public/c-send/api/v1/index.php
sudo -u www-data env APP_CONFIG_FILE=/etc/csend-template/config.php php /home/web/c-send-private/bin/encrypt-email-data.php
sudo -u postgres psql -d csend_templates -v ON_ERROR_STOP=1 -f "$STAGE/database/migrations/014_email_encryption_indexes.sql"
sudo -u www-data env APP_CONFIG_FILE=/etc/csend-template/config.php php /home/web/c-send-private/bin/verify-email-login.php
email_plaintext=$(sudo -u postgres psql -d csend_templates -Atqc "SELECT count(*) FROM (SELECT email FROM users WHERE email IS NOT NULL UNION ALL SELECT email FROM google_accounts UNION ALL SELECT email FROM email_auth_challenges UNION ALL SELECT email FROM email_auth_sends) addresses WHERE email NOT LIKE 'enc:v1:%'")
if [ "$email_plaintext" != "0" ]; then echo "email_encryption_verification=failed"; exit 1; fi
echo "email_encryption_verification=passed"
sudo install -o root -g root -m 0644 "$STAGE/nginx-c-send-locations.conf" /etc/nginx/snippets/csend-template.conf
sudo nginx -t
sudo systemctl reload nginx
for file in "$STAGE"/public/*.php; do sudo php -l "/home/web/public/c-send/t/$(basename "$file")"; done
for file in "$STAGE"/app/*.php; do sudo php -l "/home/web/c-send-private/app/$(basename "$file")"; done
sudo php -l /home/web/c-send-private/bin/encrypt-email-data.php
sudo php -l /home/web/c-send-private/bin/verify-email-login.php
sudo php -l /home/web/public/c-send/api/v1/index.php
echo "deployment=complete"
echo "backup=$BACKUP"
'@
try {
    [System.IO.File]::WriteAllText($remoteScript, $script.Replace("`r`n", "`n"), [System.Text.UTF8Encoding]::new($false))
    $sshConfigArgs = @()
    if ($SshConfig) { $sshConfigArgs = @('-F', $SshConfig) }
    & tar.exe -czf $archive -C $stage .
    if ($LASTEXITCODE -ne 0) { throw 'archive creation failed' }
    & scp.exe @sshConfigArgs $archive "${SshTarget}:/tmp/csend-template-ui.tar.gz"
    if ($LASTEXITCODE -ne 0) { throw 'archive transfer failed' }
    & scp.exe @sshConfigArgs $remoteScript "${SshTarget}:/tmp/csend-template-ui-deploy.sh"
    if ($LASTEXITCODE -ne 0) { throw 'deployment script transfer failed' }
    & ssh.exe @sshConfigArgs $SshTarget 'bash /tmp/csend-template-ui-deploy.sh'
    if ($LASTEXITCODE -ne 0) { throw 'remote deployment failed' }
} finally {
    if (Test-Path -LiteralPath $deployDir) { Remove-Item -LiteralPath $deployDir -Recurse -Force }
}
