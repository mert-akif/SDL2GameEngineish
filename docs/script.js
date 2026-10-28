'use strict';

const menuButton = document.querySelector('.menu-toggle');
const mobileNav = document.querySelector('.mobile-nav');

function closeMenu() {
  if (!menuButton || !mobileNav) return;
  menuButton.setAttribute('aria-expanded', 'false');
  menuButton.setAttribute('aria-label', 'Open navigation menu');
  mobileNav.hidden = true;
}

if (menuButton && mobileNav) {
  menuButton.addEventListener('click', () => {
    const opening = menuButton.getAttribute('aria-expanded') !== 'true';
    menuButton.setAttribute('aria-expanded', String(opening));
    menuButton.setAttribute('aria-label', opening ? 'Close navigation menu' : 'Open navigation menu');
    mobileNav.hidden = !opening;
  });
  mobileNav.querySelectorAll('a').forEach(link => link.addEventListener('click', closeMenu));
  document.addEventListener('keydown', event => {
    if (event.key === 'Escape') closeMenu();
  });
  document.addEventListener('click', event => {
    if (!mobileNav.hidden && !menuButton.contains(event.target) && !mobileNav.contains(event.target)) closeMenu();
  });
  window.matchMedia('(min-width: 851px)').addEventListener('change', event => {
    if (event.matches) closeMenu();
  });
}

const reduceMotion = window.matchMedia('(prefers-reduced-motion: reduce)').matches;
if (!reduceMotion && 'IntersectionObserver' in window) {
  const observer = new IntersectionObserver((entries) => {
    for (const entry of entries) {
      if (!entry.isIntersecting) continue;
      entry.target.classList.add('is-visible');
      observer.unobserve(entry.target);
    }
  }, { threshold: 0.08, rootMargin: '0px 0px -25px 0px' });
  document.querySelectorAll('.reveal').forEach(item => {
    item.classList.add('will-reveal');
    observer.observe(item);
  });
}

const dialog = document.getElementById('screenshot-dialog');
if (dialog && typeof dialog.showModal === 'function') {
  const lightboxImage = dialog.querySelector('img');
  const lightboxCaption = dialog.querySelector('.lightbox-caption');
  document.querySelectorAll('[data-lightbox-src]').forEach(button => {
    button.addEventListener('click', () => {
      lightboxImage.src = button.dataset.lightboxSrc;
      lightboxImage.alt = button.dataset.lightboxAlt || 'Full screenshot';
      lightboxCaption.textContent = lightboxImage.alt;
      dialog.showModal();
    });
  });
  dialog.querySelector('.lightbox-close').addEventListener('click', () => dialog.close());
  dialog.addEventListener('click', event => { if (event.target === dialog) dialog.close(); });
  dialog.addEventListener('close', () => { lightboxImage.removeAttribute('src'); });
}

async function copyText(text) {
  if (navigator.clipboard && window.isSecureContext) {
    await navigator.clipboard.writeText(text);
    return;
  }
  const helper = document.createElement('textarea');
  helper.value = text;
  helper.style.position = 'fixed';
  helper.style.opacity = '0';
  document.body.appendChild(helper);
  helper.select();
  const successful = document.execCommand('copy');
  helper.remove();
  if (!successful) throw new Error('Copy command was not successful');
}

document.querySelectorAll('[data-copy-target]').forEach(button => {
  button.addEventListener('click', async () => {
    const code = document.getElementById(button.dataset.copyTarget);
    if (!code) return;
    try {
      await copyText(code.textContent.trim());
      button.textContent = 'Copied!';
    } catch (_) {
      button.textContent = 'Select text';
    }
    button.classList.add('copied');
    window.setTimeout(() => { button.textContent = 'Copy'; button.classList.remove('copied'); }, 1800);
  });
});

const year = document.getElementById('current-year');
if (year) year.textContent = String(new Date().getFullYear());

// Documentation tabs: accessible selection + keyboard navigation.
const docTabs = Array.from(document.querySelectorAll('[data-doc-tab]'));
const docPanels = Array.from(document.querySelectorAll('[data-doc-panel]'));
function activateDocTab(next, focus = false) {
  const key = next.dataset.docTab;
  docTabs.forEach(tab => {
    const selected = tab === next;
    tab.setAttribute('aria-selected', String(selected));
    tab.tabIndex = selected ? 0 : -1;
  });
  docPanels.forEach(panel => { panel.hidden = panel.dataset.docPanel !== key; });
  if (focus) next.focus();
}
docTabs.forEach((tab, index) => {
  tab.addEventListener('click', () => activateDocTab(tab));
  tab.addEventListener('keydown', event => {
    if (!['ArrowRight', 'ArrowDown', 'ArrowLeft', 'ArrowUp', 'Home', 'End'].includes(event.key)) return;
    event.preventDefault();
    let next = index;
    if (event.key === 'ArrowRight' || event.key === 'ArrowDown') next = (index + 1) % docTabs.length;
    if (event.key === 'ArrowLeft' || event.key === 'ArrowUp') next = (index + docTabs.length - 1) % docTabs.length;
    if (event.key === 'Home') next = 0;
    if (event.key === 'End') next = docTabs.length - 1;
    activateDocTab(docTabs[next], true);
  });
});

// Show reading progress and a scroll-to-top shortcut.
const progressBar = document.querySelector('.reading-progress span');
const toTopButton = document.querySelector('.back-to-top');
const navLinks = Array.from(document.querySelectorAll('.desktop-nav a'));
let scrollPending = false;
function updateScrollUi() {
  const scrollHeight = document.documentElement.scrollHeight - window.innerHeight;
  const fraction = scrollHeight > 0 ? Math.min(1, Math.max(0, window.scrollY / scrollHeight)) : 0;
  if (progressBar) progressBar.style.width = `${fraction * 100}%`;
  if (toTopButton) toTopButton.classList.toggle('show', window.scrollY > 520);
  scrollPending = false;
}
window.addEventListener('scroll', () => {
  if (scrollPending) return;
  scrollPending = true;
  window.requestAnimationFrame(updateScrollUi);
}, { passive: true });
window.addEventListener('resize', updateScrollUi);
updateScrollUi();

if ('IntersectionObserver' in window && navLinks.length) {
  const sections = navLinks.map(a => document.querySelector(a.getAttribute('href'))).filter(Boolean);
  const observer = new IntersectionObserver(entries => {
    entries.forEach(entry => {
      if (!entry.isIntersecting) return;
      navLinks.forEach(link => link.classList.toggle('active', link.getAttribute('href') === `#${entry.target.id}`));
    });
  }, { rootMargin: '-15% 0px -65% 0px', threshold: 0 });
  sections.forEach(section => observer.observe(section));
}
