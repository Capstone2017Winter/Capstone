$(document).ready(function(){
	classes = new Array();
	$('#addClassButton').click(function(){
		var classNameInput = $("#classSearchTextId").val();
		classes.push(classNameInput);
		$('#classListId').append('<li><p>' + classNameInput + "</p></li>");
	});
});
