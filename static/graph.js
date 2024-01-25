function submitForm() {
  document.getElementById("stocks").submit();
  document.getElementById("options").submit();
  document.getElementById("time").submit();
  document.getElementById("stock 1").submit();
  document.getElementById("stock 2").submit();

}
function toggleMode(mode) {
  if (mode === 1) {
      document.getElementById('mode1Content').style.display = 'block';
      document.getElementById('mode2Content').style.display = 'none';
  } else {
      document.getElementById('mode1Content').style.display = 'none';
      document.getElementById('mode2Content').style.display = 'block';
  }
}